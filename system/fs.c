#include <xinu.h>
#include <kernel.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>


#ifdef FS
#include <fs.h>

static struct fsystem fsd;
int dev0_numblocks;
int dev0_blocksize;
char *dev0_blocks;

extern int dev0;

char block_cache[512];

#define SB_BLK 0
#define BM_BLK 1
#define RT_BLK 2

#define NUM_FD 16
struct filetable oft[NUM_FD];
int next_open_fd = 0;


#define INODES_PER_BLOCK (fsd.blocksz / sizeof(struct inode))
#define NUM_INODE_BLOCKS (( (fsd.ninodes % INODES_PER_BLOCK) == 0) ? fsd.ninodes / INODES_PER_BLOCK : (fsd.ninodes / INODES_PER_BLOCK) + 1)
#define FIRST_INODE_BLOCK 2

int fs_fileblock_to_diskblock(int dev, int fd, int fileblock);

/* YOUR CODE GOES HERE */
// Global current inode numebr
int inodeNum = 0;
int setMask = FIRST_INODE_BLOCK;

int fs_fileblock_to_diskblock(int dev, int fd, int fileblock) {
  int diskblock;

  if (fileblock >= INODEBLOCKS - 2) {
    printf("No indirect block support\n");
    return SYSERR;
  }

  diskblock = oft[fd].in.blocks[fileblock]; //get the logical block address

  return diskblock;
}

/* read in an inode and fill in the pointer */
int
fs_get_inode_by_num(int dev, int inode_number, struct inode *in) {
  int bl, inn;
  int inode_off;

  if (dev != 0) {
    printf("Unsupported device\n");
    return SYSERR;
  }
  if (inode_number > fsd.ninodes) {
    printf("fs_get_inode_by_num: inode %d out of range\n", inode_number);
    return SYSERR;
  }

  bl = inode_number / INODES_PER_BLOCK;
  inn = inode_number % INODES_PER_BLOCK;
  bl += FIRST_INODE_BLOCK;

  inode_off = inn * sizeof(struct inode);

  /*
  printf("in_no: %d = %d/%d\n", inode_number, bl, inn);
  printf("inn*sizeof(struct inode): %d\n", inode_off);
  */

  bs_bread(dev0, bl, 0, &block_cache[0], fsd.blocksz);
  memcpy(in, &block_cache[inode_off], sizeof(struct inode));

  return OK;

}

int
fs_put_inode_by_num(int dev, int inode_number, struct inode *in) {
  int bl, inn;

  if (dev != 0) {
    printf("Unsupported device\n");
    return SYSERR;
  }
  if (inode_number > fsd.ninodes) {
    printf("fs_put_inode_by_num: inode %d out of range\n", inode_number);
    return SYSERR;
  }

  bl = inode_number / INODES_PER_BLOCK;
  inn = inode_number % INODES_PER_BLOCK;
  bl += FIRST_INODE_BLOCK;

  /*
  printf("in_no: %d = %d/%d\n", inode_number, bl, inn);
  */

  bs_bread(dev0, bl, 0, block_cache, fsd.blocksz);
  memcpy(&block_cache[(inn*sizeof(struct inode))], in, sizeof(struct inode));
  bs_bwrite(dev0, bl, 0, block_cache, fsd.blocksz);

  return OK;
}

int fs_mkfs(int dev, int num_inodes) {
  int i;

  // Check if the device given is default device
  if (dev == 0) {
    // Set the number of blocks to dev0's number of blocks
    // #define MDEV_NUM_BLOCKS 512 | fs.h:18
    fsd.nblocks = dev0_numblocks;
    // Set the block size of the filesystem the same size as dev0's block size
    // #define MDEV_BLOCK_SIZE 512 | fs.h:17
    fsd.blocksz = dev0_blocksize;
  }
  else {
    printf("Unsupported device\n");
    return SYSERR;
  }

  if (num_inodes < 1) {
    // #define DEFAULT_NUM_INODES (MDEV_NUM_BLOCKS / 4) | fs.h:19
    fsd.ninodes = DEFAULT_NUM_INODES;
  }
  else {
    fsd.ninodes = num_inodes;
  }

  // Set i to the number of blocks within the file system
  // #define MDEV_NUM_BLOCKS 512 | fs.h:18 
  i = fsd.nblocks;
  // Loop through incrementing i untilreaching an i that is divisible by 8
  while ( (i % 8) != 0) {i++;}
  // i / 8 will give the total number of bytes needed for the free mask
  fsd.freemaskbytes = i / 8;

  // Allocate memory for the free mask array
  if ((fsd.freemask = getmem(fsd.freemaskbytes)) == (void *)SYSERR) {
    printf("fs_mkfs memget failed.\n");
    return SYSERR;
  }

  /* zero the free mask */
  for(i=0;i<fsd.freemaskbytes;i++) {
    fsd.freemask[i] = '\0';
  }

  // Set the number of inodes used to zero since no inodes have been created yet
  fsd.inodes_used = 0;

  /* write the fsystem block to SB_BLK, mark block used */
  fs_setmaskbit(SB_BLK);
  bs_bwrite(dev0, SB_BLK, 0, &fsd, sizeof(struct fsystem));

  /* write the free block bitmask in BM_BLK, mark block used */
  fs_setmaskbit(BM_BLK); // Set bitmask in block 1 to freemask array
  bs_bwrite(dev0, BM_BLK, 0, fsd.freemask, fsd.freemaskbytes); // commit the freemask to block 1 in the filesystem

  return 1;
}

void
fs_print_fsd(void) {

  printf("fsd.ninodes: %d\n", fsd.ninodes);
  printf("sizeof(struct inode): %d\n", sizeof(struct inode));
  printf("INODES_PER_BLOCK: %d\n", INODES_PER_BLOCK);
  printf("NUM_INODE_BLOCKS: %d\n", NUM_INODE_BLOCKS);
}

/* specify the block number to be set in the mask */
int fs_setmaskbit(int b) {
  int mbyte, mbit;
  mbyte = b / 8;
  mbit = b % 8;

  fsd.freemask[mbyte] |= (0x80 >> mbit);
  return OK;
}

/* specify the block number to be read in the mask */
int fs_getmaskbit(int b) {
  int mbyte, mbit;
  mbyte = b / 8;
  mbit = b % 8;

  return( ( (fsd.freemask[mbyte] << mbit) & 0x80 ) >> 7);
  return OK;

}

/* specify the block number to be unset in the mask */
int fs_clearmaskbit(int b) {
  int mbyte, mbit, invb;
  mbyte = b / 8;
  mbit = b % 8;

  invb = ~(0x80 >> mbit);
  invb &= 0xFF;

  fsd.freemask[mbyte] &= invb;
  return OK;
}

/* This is maybe a little overcomplicated since the lowest-numbered
   block is indicated in the high-order bit.  Shift the byte by j
   positions to make the match in bit7 (the 8th bit) and then shift
   that value 7 times to the low-order bit to print.  Yes, it could be
   the other way...  */
void fs_printfreemask(void) {
  int i,j;

  for (i=0; i < fsd.freemaskbytes; i++) {
    for (j=0; j < 8; j++) {
      printf("%d", ((fsd.freemask[i] << j) & 0x80) >> 7);
    }
    if ( (i % 8) == 7) {
      printf("\n");
    }
  }
  printf("\n");
}


int fs_open(char *filename, int flags) {
  int i;
  // Loop the total number of files allowed in the open file table
  for (i = 0; i < NUM_FD; i++) {
      
    // If we find the filename in the open file table
    if(strcmp(oft[i].de->name, filename) == 0) {
      // Set the file to the open file state
      oft[i].state = FSTATE_OPEN; 
      // Set the given flags
      oft[i].flag = flags;
      return i;
    } 
  }  
  printf("The file %s was not found\n",filename);
  return SYSERR;
}

int fs_close(int fd) {
  // Check if the file descriptor is greater than te allowed number of open files
  if(fd < NUM_FD){ 
    // The the files in the open file table to the file closed state
    oft[fd].state = FSTATE_CLOSED;
    return OK;
  }
  return SYSERR;
}

int fs_create(char *filename, int mode) {
  // Determine if the directory is full
  if (next_open_fd < DIRECTORY_SIZE) {
      
    // Create directory and fill in file data for it
    oft[next_open_fd].de = (struct dirent *)getmem(sizeof(struct dirent));
    oft[next_open_fd].de->inode_num = inodeNum;
    strcpy(oft[next_open_fd].de->name, filename);
      
    // Create the file inode
    oft[next_open_fd].in.id = inodeNum;
    oft[next_open_fd].in.type = INODE_TYPE_FILE;
    // Only device we have is dev0
    oft[next_open_fd].in.device = dev0;
    oft[next_open_fd].in.size = 0;
  
    // Update the root directory data
    fsd.root_dir.numentries = inodeNum + 1; // increment number of inodes and store as the total number of entries in the root directory
    fsd.root_dir.entry[inodeNum+1] = *oft[next_open_fd].de; // Set root directory entry to a pointer to the new directory entry
      
    // 
    if ((inodeNum % 8 == 0) && (inodeNum != 0)) {
      
      fs_setmaskbit(setMask);
      setMask++; 
    }
      
    /* put the inode in memory */
    if(fs_put_inode_by_num(dev0, inodeNum, &oft[next_open_fd].in) == 1) {
      inodeNum++;
      return next_open_fd++;
    }
           
  } 
  return SYSERR;
}

int fs_seek(int fd, int offset) {
  // Add in error checking
  return oft[fd].fileptr + offset;
}

int fs_read(int fd, void *buf, int nbytes) {
  int inode,k,blksToRead,n_blk;
  struct inode in;
  char data[MDEV_BLOCK_SIZE];
  int fptr,i,temp;
  
  // Get inode number from the file descriptor given
  inode = oft[fd].de->inode_num;
  // Get the inode by the inode returned above
  fs_get_inode_by_num(0, inode, &in);
  // Get the file pointer to the file to be read
  fptr = oft[fd].fileptr;
 
  temp = nbytes;
  n_blk = nbytes/512;

  // Loop through the blocks
  for(i=0;i<=n_blk;i++) {
    // Get the total number of bytes to read
    blksToRead = (nbytes < MDEV_BLOCK_SIZE) ? nbytes : MDEV_BLOCK_SIZE;
    // Read the entire block
    bs_bread(0, oft[fd].in.blocks[i], 0, data,blksToRead);
    // Copy the data into the buffer
    strcpy(buf,data);
    // Remove the blocks to read from the number of bytes to read
    temp -= blksToRead;
    // Check if the total bytes to read have been read
    if(temp==0)
      break;
  }

  fptr = oft[fd].fileptr + nbytes;
  return fptr;
}

int fs_write(int fd, void *buf, int nbytes) {
  //int numBlocks = NUM_INODE_BLOCKS;//nbytes/512;
  int dataMask = 17;
  int offset = 0;
  int blockIndex = 0;
  int blockToWrite = 0;
  int i = 0;
  int bytesToWrite;

  // Check if the flag for read only is set
  if (oft[fd].flag == O_RDONLY)
    return SYSERR;

  // Get the mask bit where new data can be written
  while(fs_getmaskbit(dataMask) != 0) {
    dataMask++;
  }
  
  // 
  if (oft[fd].in.size <= 512) {
    blockIndex = 0;
  } else {
    blockIndex = (oft[fd].in.size/512);                
  }

  while (nbytes > 0) {
    if(i > 0) {
      buf = buf + bytesToWrite;
    }
    if ((oft[fd].fileptr % 512) == 0 ) {
      blockToWrite = dataMask;
      fs_setmaskbit(blockToWrite);
      dataMask++;
    } else {
      blockToWrite = oft[fd].in.blocks[blockIndex];
    }
    if (blockIndex == 0) {
      bytesToWrite = 512 - oft[fd].fileptr;
      offset = oft[fd].fileptr;
            
      if (nbytes < bytesToWrite) {
	bytesToWrite = nbytes;
      }
    } else {
      bytesToWrite = ((blockIndex+1)*512) - oft[fd].fileptr;
      offset = ((blockIndex)*512) - bytesToWrite;
      if (i > 0) {
	offset = 0;
      }
      if (nbytes < bytesToWrite) {
	bytesToWrite = nbytes;
      }
    }


    if (bs_bwrite(dev0, blockToWrite, offset, buf, bytesToWrite) != 1) {
      return SYSERR;
    }

    nbytes = nbytes - bytesToWrite;
    oft[fd].fileptr = oft[fd].fileptr + bytesToWrite;
    blockIndex++;
    oft[fd].in.blocks[blockIndex] = dataMask;
                
  }
  oft[fd].in.size = oft[fd].fileptr;
  return oft[fd].fileptr;
}

#endif /* FS */
