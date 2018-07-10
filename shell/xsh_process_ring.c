/* xsh_process_ring.c - xsh_process_ring */

#include <xinu.h>
#include <stdlib.h> /* strtol */
#include <stdio.h>
#include <ctype.h> /* is_digit */
#include <process_ring.h>

/*------------------------------------------------------------------------
 * xsh_process_ring - coordinates processes to count from some number down
 * to zero
 *------------------------------------------------------------------------
 */

void argument_error(char *command) {
  fprintf(stderr, "%s: invalid argument\n", command);
  fprintf(stderr, "Try '%s --help' for more information\n", command);
}

void range_error(char *command) {
  fprintf(stderr, "%s: invalid process number (0-64)\n", command);
  fprintf(stderr, "Try '%s --help' for more information\n", command);
}

void print_help(char *command) {
  printf("\nSYNOPSIS:\n");
  printf("\t%s [-p <0-64>] [-r <uint32>] [-i <poll or sync>]\n", command);
  printf("\t%s <0-64> [<uint32>]\n\n", command);
  printf("DESCRIPTION:\n");
  printf("\tcoordinates processes to count from some number\n");
  printf("\tdown to zero.\n");
  printf("OPTIONS:\n");
  printf("\t-p \t\tnumber of processes (default 2)\n");
  printf("\t-r <uint32>\t\tnumber of rounds (default 3)\n");
  printf("\t-i <poll or sync>\t\timplementation choice. default poll\n");
  printf("\t--help\t display this help and exit\n\n");
}

shellcmd xsh_process_ring(int nargs, char *args[]) {
  
  char *endp;
  int32 p = 2; /* Number of processes */
  int32 r = 3; /* Number of rounds */
  int32 i = 0; /* Implementation choice: poll(0) sync(1) */
  int32 x;     /* For loop index */
  
  /* For argument '--help', emit help about the 'memdump' command */
  if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
    print_help(args[0]);
    return 0;
  }
  
  /* Check argument count */
  if (nargs > 7) {
    fprintf(stderr, "%s: too many arguments\n", args[0]);
    fprintf(stderr, "Try '%s --help' for more information\n", args[0]);
    return 1;
  }
  
  /* Loop on coimmand line arguments */
  for (x = 1; x < nargs; x++) {
    if (strncmp("-p", args[x], 3) == 0) {
      if (!(x+1 < nargs)) {
	  printf("-p Expected argument\n");
	  return 1;
      }
      /* Set number of processes */
      p = strtol(args[x+1],&endp,10);
      if (endp == args[x+1] || *endp != '\0') {
	/* Expected an integer OR Garbage string */
	return 1;
      } else if (!(0 <= p && p <= 64)) {
	/* Out of range */
	return 1;
      }
      x++;
    } else if (strncmp("-r", args[x], 3) == 0) {
      if (!(x+1 < nargs)) {
	printf("-r expected argument\n");
	return 1;
      }
      /* Set the number of rounds */
      r = strtol(args[x+1],&endp,10);
      if (endp == args[x+1] || *endp != '\0') {
	/* Expected an integer OR Garbage string */
	return 1;
      }
      x++;
    } else if (strncmp("-i",args[x], 3) == 0) {
      if (!(x+1 < nargs)) {
	printf("-i expected argument\n");
	return 1;
      } 
      /* Set implementation style */
      if (strncmp("poll",args[x+1],5) == 0) {
	/* Do nothing, poll already default */
      } else if (strncmp("sync",args[x+1],5) == 0) {
	i = 1;
      } else {
	printf("-i takes either poll or sync as argument.\n");
	return 1;
      }
      x++;
    } else if (x == 1) {
      /* Set number of processes */
      p = strtol(args[x],&endp,10);
      if (endp == args[x] || *endp != '\0') {
	/* Expected an integer OR Garbage string */
	return 1;
      } else if (!(0 <= p && p <= 64)) {
        /* Out of range */
        return 1;
      }
    } else if (x == 2) {
      /* Set number of rounds */
      r = strtol(args[x],&endp,10);
      if (endp == args[x] || *endp != '\0') {
        /* Expected an integer OR Garbage string */
        return 1;
      }
    } else {
      argument_error(args[x]);
    }
  }
  
  printf("Number of Processes: %d\n", p);
  printf("Number of Rounds: %d\n", r);
  
  /* Start timer */
  uint32 start;
  gettime(&start);
  
  if (i) {
    /* sync */
    sync_implementation(r,p);
  } else {
    /* poll */
    poll_implementation(r,p);
  }
  
  
  /* End timer */
  uint32 end;
  gettime(&end);
  printf("Elapsed Seconds: %d\n", end - start);
  return 0;
}
