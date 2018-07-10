#include <xinu.h>
#include <string.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * xsh_producer_consumer - example of producer consumer implementation 
 * (updated for futures)
 *------------------------------------------------------------------------
 */

void print_prodcons_help() {
  printf("producer-consumer <buffer size in bytes> <bytes to send> <max delay> [-f]\n");
}

shellcmd xsh_producer_consumer(int nargs, char *args[]) {
  /* Check for required amount of command line arguments */
  if (nargs != 4 && nargs != 2) {
    print_prodcons_help();
    return 1;
  }

  /* Check for future flag */
  if (nargs == 2) {
    if (strcmp(args[1], "-f") == 0) {
      /* Future flag set 
         run future test code */
      future_t *f_exclusive, *f_shared, *f_queue;
 
      f_exclusive = future_alloc(FUTURE_EXCLUSIVE);
      f_shared    = future_alloc(FUTURE_SHARED);
      f_queue     = future_alloc(FUTURE_QUEUE);
 
      /* Test FUTURE_EXCLUSIVE */
      resume( create(future_cons, 1024, 20, "fcons1", 1, f_exclusive));
      resume( create(future_prod, 1024, 20, "fprod1", 2, f_exclusive,1));

      /* Test FUTURE_SHARED
      resume( create(future_cons, 1024, 20, "fcons2", 1, f_shared));
      resume( create(future_cons, 1024, 20, "fcons3", 1, f_shared));
      resume( create(future_cons, 1024, 20, "fcons4", 1, f_shared)); 
      resume( create(future_cons, 1024, 20, "fcons5", 1, f_shared));
      resume( create(future_prod, 1024, 20, "fprod2", 2, f_shared,2));
      /*
      /* Test FUTURE_QUEUE
      resume( create(future_cons, 1024, 20, "fcons6", 1, f_queue));
      resume( create(future_cons, 1024, 20, "fcons7", 1, f_queue));
      resume( create(future_cons, 1024, 20, "fcons8", 1, f_queue));
      resume( create(future_cons, 1024, 20, "fcons9", 1, f_queue));
      resume( create(future_prod, 1024, 20, "fprod3", 2, f_queue,3));
      resume( create(future_prod, 1024, 20, "fprod4", 2, f_queue,4));
      resume( create(future_prod, 1024, 20, "fprod5", 2, f_queue,5));
      resume( create(future_prod, 1024, 20, "fprod6", 2, f_queue,6));
      */
    }
    /* Futures ran, return */
    return SHELL_OK;
  }

  /* Get command line arguments */
  int32 buffersize = atoi(args[1]);
  int32 total_bytes = atoi(args[2]);
  int32 max_delay = atoi(args[3]);

  /* Set up semaphores to be used */
  sid32 mutex = semcreate(1);
  sid32 items = semcreate(0);
  sid32 spaces = semcreate(buffersize);
  sid32 done_sem = semcreate(0);

  /* Create producer and consumer threads */
  resume(create(consumer, 1024, 20, "consumer", 6,mutex,items,spaces,done_sem,total_bytes,max_delay));
  resume(create(producer, 1024, 20, "producer", 6,mutex,items,spaces,done_sem,total_bytes,max_delay));
  
  /* Set up waits for semaphores 
     This allows the threads to finish before main process ends */
  wait(done_sem);
  wait(done_sem);
  /* Clean up semaphores */
  semdelete(mutex);
  semdelete(items);
  semdelete(spaces);

  return SHELL_OK;
}

process producer(sid32 mutex, sid32 items, sid32 spaces, sid32 done_sem,
		 int32 total_bytes, int32 max_delay) {

  /* Set the initial value of sent bytes */
  int32 bytes_produced = 0;

  /* Loop until all bytes to be sent have been sent */
  while (bytes_produced != total_bytes) {

    /* Wait for event - Random process delay */
    rand_delay(max_delay);
 
    wait(spaces); /* Wait for space to be available */
    wait(mutex);  /* Wait for mutex to be unlocked */

    /**** CRITICAL SECTION ****/
    
    /* Increment bytes consumed so total bytes is reached at some point */
    bytes_produced++;
    printf("Producer: 0x%x\n", bytes_produced % 256);

    /**** END CRITICAL SECTION ****/

    signal(mutex); /* Signal mutex to unlock it for consumer */
    signal(items); /* Signal items to acknowledge item available */

  }
  
  /* Signal done sem to notify parent proces of completion */
  signal(done_sem);
  return OK;
}

process consumer(sid32 mutex, sid32 items, sid32 spaces, sid32 done_sem,
		 int32 total_bytes, int32 max_delay) {
  
  /* Set the initial value of received bytes */
  int32 bytes_consumed = 0;

  /* Loop until all bytes have been received */
  while (bytes_consumed != total_bytes) {

    wait(items); /* Wait for item to be available */
    wait(mutex); /* Wait for mutex to be unlocked */

    /**** CRITICAL SECTION ****/
 
    /* Incrmement bytes consumed so total bytes is reached at some point */
    bytes_consumed++;
    printf("Consumer: 0x%x\n", bytes_consumed%256);

    /**** END CRITICAL SECTION ****/
    
    signal(mutex);  /* Signal mutex to unlock it for producer */
    signal(spaces); /* Signal spaces to acknowledge space available */

    /* Process item - Random process delay */
    rand_delay(max_delay);

  }
  
  /* Signal done sem to notify parent proces of completion */
  signal(done_sem);
  return OK;
}

