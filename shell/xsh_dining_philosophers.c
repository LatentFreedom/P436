#include <xinu.h>
#include <string.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * xsh_producer_consumer - example of producer consumer implementation
 *------------------------------------------------------------------------
 */


void print_dining_help() {
  printf("dining-philosophers <num philosophers> <num cycles> <max delay>");
}

shellcmd xsh_dining_philosophers(int nargs, char *args[]) {
  
  int32 i;
  sid32 forks[100];

  /* Check for required command line arguments */
  if (nargs != 4) {
    print_dining_help();
    return 1;
  }

  
  /* Get command line arguments */
  int32 n_philosophers = atoi(args[1]);
  int32 n_cycles = atoi(args[2]);
  int32 max_delay = atoi(args[3]);

  /* Set up semaphores to be used 
     There will be the same amount of forks as philosophers */
  for (i = 0; i < n_philosophers; i++) {
    /* forks are a mutex becasue they will be ready to use at the start */
    forks[i] = semcreate(1);
  }
  sid32 footman = semcreate(n_philosophers-1);
  sid32 done_sem = semcreate(0);
 
  /* Create philosopher threads */
  for (i = 0; i < n_philosophers; i++) {
    resume(create(philosopher, 1024, 20, "philosopher", 7,n_philosophers,forks,n_cycles,footman,done_sem,max_delay,i));
  }

  /* Set up waits for semaphores 
     This allows the threads to finish before main process ends */
  for (i = 0; i < n_philosophers; i++) {
    wait(done_sem);
  }
 
  /* Clean up semaphores */
  for (i = 0; i < n_philosophers; i++) {
    /* delete fork semaphore */
    semdelete(forks[i]);
  }

  semdelete(done_sem);

  return SHELL_OK;
}

int32 left(int32 i) {
  return i;
}

/* Return the fork on the right side given i and number of philosophers */
int32 right(int32 i, int32 n_philosophers) {
  /* mod philosophers so i is never more than the total philosophers */
  return (i + 1) % n_philosophers;
}

/* Get the forks to use - wait for the given fork semaphores */
void get_forks(int32 i, sid32 footman, int32 n_philosophers, sid32 forks[]) {
  wait(footman);
  wait(forks[right(i,n_philosophers)]);/* Wait for the right fork to be available */
  wait(forks[left(i)]);/* Wait for the left fork to be available */
}

/* Put the forks back - signal the given fork semaphores */
void put_forks(int32 i, sid32 footman, int32 n_philosophers, sid32 forks[]) {
  /* Signal that the given forks are available */
  signal(forks[right(i,n_philosophers)]);
  signal(forks[left(i)]);
  signal(footman);
}

process philosopher(int32 n_philosophers, sid32 forks[], int32 n_cycles, sid32 footman, sid32 done_sem, int32 max_delay, int32 n) {

  int32 cur_cycle;

  /* Loop for the given amount of cycles */
  for (cur_cycle = 0; cur_cycle < n_cycles; cur_cycle++) {


    /**** THINKING *******/
    rand_delay(max_delay);
    printf("Philosopher %d thinking %d\n",n,cur_cycle);
    /*********************/

    /* Get the forks to begin eating */
    get_forks(n, footman, n_philosophers, forks);

    /**** EATING *********/
    rand_delay(max_delay);
    printf("Philosopher %d eating %d\n",n,cur_cycle);
    /*********************/

    /* Put the forks back and continue */
    put_forks(n, footman, n_philosophers, forks);

   
  }
  
  /* Signal done sem to notify parent proces of completion */
  signal(done_sem);
  return OK;
}
