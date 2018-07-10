#include <string.h>
#include <stdio.h>
#include <stdlib.h>
// Needed for pid_t
#include <unistd.h>
#include <sys/types.h>

/*------------------------------------------------------------------------                                                                                      
 * sys_info                                                                                                                                                     
- create 2 processes using fork() system call.                                                                                                                  
- Each process should print it's process id and whether it is the parent or child process.                                                                      
- Parent process will send a string argument to child process using pipe()(write end of pipe). The string written to the pipe is the argument given to the prog\
ram on the command line. Ex: date, uname, hostname, echo.                                                                                                       
- Child process will receive the string argument from parent process through read end of the pipe.                                                              
- Child process should use execl() to run the program for the corresponding argument sent by parent process.                                                    
*------------------------------------------------------------------------                                                                                      
*/

#define READ 0 /* Read end of pipe */
#define WRITE 1 /* Write end of pipe */

int main(int argc, char *argv[], char *env[]) {
  int fd[2], ret;
  pid_t pid;

  // Check for correct amount of arguments
  if (argc < 2) {
    printf("too few arguments.\n");
    return 0;
  } else if (argc > 2) {
    printf("too many arguments.\n");
    return 0;
  }  
  char * string = argv[1];
  char com[50];
  strcpy(com, "/bin/");
  strcat(com , string);

  ret = pipe(fd);
  if (ret == -1) {
    perror("pipe");
    exit(1);
  }

  pid = fork();
  if (pid < 0) {
    perror("fork");
    exit(1);
  }
  if(pid == 0) {
    /* Child Process */
    printf("Child PID: %d\n", getpid());
    close(fd[WRITE]);
    read(fd[READ], com, 20);
    close(fd[READ]);
    if (!strcmp(string,"echo")) {
      execl(com, com, "Hello World!", NULL);
    } else {
      execl(com, com, NULL, NULL);
    }
  } else {
    /* Parent Process */
    printf("Parent PID: %d\n",getpid());
    close(fd[READ]);
    write(fd[WRITE], com, strlen(string)+1);
    close(fd[WRITE]);
  }
}
