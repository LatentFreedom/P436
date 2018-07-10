#include <xinu.h>
#include <string.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * xsh_readers_writers - implementation for readers writer problem
 *------------------------------------------------------------------------
 */


void print_help2() {
  printf("readers-writers <num writers> <num readers> <write cycles> <read cycles> <max delay>\n\r");
}

shellcmd xsh_readers_writers(int nargs, char *args[]) {
  
  int32 i;

  /* Check for required amount of command line arguments */
  if (nargs != 6) {
    print_help2();
    return 1;
  }

  
  /* Get command line arguments */
  int32 n_writers = atoi(args[1]);
  int32 n_readers = atoi(args[2]);
  int32 w_cycles = atoi(args[3]);
  int32 r_cycles = atoi(args[4]);
  int32 max_delay = atoi(args[5]);

  /* Set up semaphores to be used */
  sid32 mutex = semcreate(1);
  sid32 room_empty = semcreate(1);
  sid32 done_sem = semcreate(0);

  /* Create global count of readers in the room */
  int32 *readers;
  readers = 0;

  /* Create reader threads */
  for (i = 0; i < n_readers; i++) {
    resume(create(reader,1024,20,"reader",6,&readers,mutex,room_empty,done_sem,r_cycles,max_delay));
  }

  /* Create writer threads */
  for (i = 0; i < n_writers; i++) {
    resume(create(writer,1024,20,"writer",4,room_empty,done_sem,w_cycles,max_delay));
  }

  /* Set up waits for semaphores
     This allows the threads to finish before main process ends */
  int32 total_threads = n_readers + n_writers;
  for (i = 0; i < total_threads; i++) {
    wait(done_sem);
  }

  /* Clean up semaphores */
  semdelete(mutex);
  semdelete(room_empty);
  semdelete(done_sem);

  return SHELL_OK;
}

process writer(sid32 room_empty, sid32 done_sem,
		 int32 w_cycles, int32 max_delay) {

  int32 cur_cycle;
  /* Loop for the given number of write cycles */
  for  (cur_cycle = 0; cur_cycle < w_cycles; cur_cycle++) {
 
    wait(room_empty); /* Wait for the room to be empty */

    /**** CRITICAL SECTION ****/

    /* Wait for event - Random process delay */
    rand_delay(max_delay);
    printf("Writer %d %d\n",currpid,cur_cycle);

    /**** END CRITICAL SECTION ****/

    signal(room_empty); /* Signal that the room is empty */

  }
  
  /* Signal done sem to notify parent proces of completion */
  signal(done_sem);
  return OK;
}

process reader(int *readers, sid32 mutex, sid32 room_empty, sid32 done_sem,
		 int32 r_cycles, int32 max_delay) {

  int32 cur_cycle;
  /* Loop for the given amount of reader cycles */
  for (cur_cycle = 0; cur_cycle < r_cycles; cur_cycle++) {

    wait(mutex); /* Wait for mutex to be unlocked */
    *readers++; /* Increment total readers in the room */
    if (*readers == 1) {
      /* First reader in the room 
         Wait for the room to be empty */
      wait(room_empty);
    }
    /* Signal mutex to allow more readers into the room */
    signal(mutex);

    /**** CRITICAL SECTION ****/

    /* Process item - Random process delay */
    rand_delay(max_delay);
    printf("Reader %d %d\n",currpid,cur_cycle);

    /**** END CRITICAL SECTION ****/
    
    wait(mutex);  /* Wait for mutex to be unlocked */
    *readers--; /* Reader is now leaving the room */
    if (*readers == 0) {
      /* Last reader in the room
	 Signal that the room is now empty */
      signal(room_empty);
    }
    /* Signal the mutex to allow more readers to enter the room */
    signal(mutex);


  }
  
  /* Signal done sem to notify parent proces of completion */
  signal(done_sem);
  return OK;
}

