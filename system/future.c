#include <xinu.h>

future_t* future_alloc(future_mode_t mode) {
  /* Disable interrupts */
  intmask mask;
  mask = disable();

  /* Allocate memory for the new future */
  future_t* future = (future_t*)getmem(sizeof(future_t));
  
  /* Check for memory allocation errors */
  if (future == -1) {
    restore(mask);
    return SYSERR;
  }

  /* Initialize future values */
  future->value = NULL;
  future->state = FUTURE_EMPTY;
  future->mode = mode; /* Set the future mode to the given mode */
  future->pid = NULL;
  future->set_queue = NULL;
  future->get_queue = NULL;

  /* Check the mode to set the queue values */
  if (mode == FUTURE_QUEUE) {
    /* Get a new queue to be used for future queue mode */
    future->set_queue = newqueue();
  }

  /* Restore interrupts and return future */
  restore(mask);
  return future;
}


syscall future_free(future_t* f) {
  
  /* Disable interrupts */
  intmask mask;
  mask = disable();
  /* Free memory that was allocated by the future 
     Make sure the memory was freed */
  if (freemem(f, sizeof(future_t)) == SYSERR) {
    /* Error freeing memory, return error */
    restore(mask);
    return SYSERR;
  }
  
  /* Restore interrupts and return */
  restore(mask);
  return OK;
}

syscall future_get(future_t* f, int* value) {
  
  struct procent *prptr;

  /* Disable Interrupts */
  intmask mask;
  mask = disable();

  /* Check future mode */
  if (f->mode == FUTURE_EXCLUSIVE) {
    /* Check future state */
    if (f->state == FUTURE_READY) {

      /* Get the future value */
      *value = f->value;
      
    } else if (f->state == FUTURE_EMPTY) {

      f->state = FUTURE_WAITING; /* Set future to waiting because it is empty */
      f->pid = getpid(); /* Set the futures pid to the current running process's pid */
      suspend(getpid()); /* Suspend the process, it will be resumed when it has a value */
      /* When returned we will have a value so no more work needs to be done */
      *value = f->value;
    }


  }

  /* Restore interrupts and return */
  restore(mask);
  return OK;
}

syscall future_set(future_t* f, int value) {
  
  struct procent *prptr;

  /* Disable interrupts */
  intmask	mask;
  mask = disable();
  
  /* Check future mode */
  if (f->mode == FUTURE_EXCLUSIVE) {
    /* Check future state */
    if (f->state == FUTURE_EMPTY) {
      /* Set future value */
      f->value = value;
      f->state = FUTURE_READY;
      
      prptr = &proctab[f->pid];
      printf("process state: %d\n",prptr->prstate);

      /* Restore intterupts and return */
      restore(mask);
      return OK;
    } else if (f->state == FUTURE_WAITING) {
      /* Set the future value */
      f->value = value;
      f->state = FUTURE_READY;


      prptr = &proctab[f->pid];
      printf("process state: %d\n",prptr->prstate);
 
      /* Get the process pid of the consumer process which is in the future pid variable
         Resume the consumer process */
      resume(f->pid);
      /* Restore interrupts and return */
      restore(mask);
      return OK;
    }
  }
  

  /* Restore interrupts and return 
     future must already hold a value */
  restore(mask);
  return SYSERR;
}
