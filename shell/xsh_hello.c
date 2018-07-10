#include <xinu.h>
#include <string.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * xsh_hello - given a name, print a hello message
 *------------------------------------------------------------------------
 */


shellcmd xsh_hello(int nargs, char *args[]) {

  if (nargs < 2) {
    printf("too few arguments.\n");
  } else if (nargs > 2) {
    printf("too many arguments.\n");
  } else {
    char * name = args[1];
    printf("Hello, %s. Welcome to the world of Xinu!\n",name);
  }
  return 0;
}
