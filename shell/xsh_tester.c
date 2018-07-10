#include <xinu.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/*------------------------------------------------------------------------                                        
 * xsh_hello - given a name, print a hello message                                                                
 *------------------------------------------------------------------------                                        
 */


shellcmd xsh_tester(int nargs, char *args[]) {

  int32 *endp;
  int32 v = strtol("123",&endp,10);
  printf("Integer: %d\n",v);
  return 0;
}

