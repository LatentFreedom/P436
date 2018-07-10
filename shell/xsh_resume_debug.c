#include <xinu.h>
#include <string.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * xsh_hello - given a name, print a hello message
 *------------------------------------------------------------------------
 */

void do_nothing(void) {
  /* Do nothing */
  sleep(50);
}

shellcmd xsh_resume_debug(int nargs, char *args[]) {

  int i;

  /* Loop to create and resume i processes to debug resume prio local variable need */
  for (i = 1; i < 100; i++) {
    resume(create(do_nothing, 1024, i%20, "do_nothing", 0));
    sleepms(1);
  }

  printf("Done\n\r");
  return 0;
}
