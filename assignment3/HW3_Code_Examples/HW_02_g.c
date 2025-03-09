// Program HW_02_g.c
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char **argv)

{ pid_t fork_returned_pid;

  fork_returned_pid = fork();

  if (fork_returned_pid < 0)
     { printf("The fork() didn't work! Terminate\n");
       exit (0); }

  if (fork_returned_pid != 0)
     { printf("The parent will now wait\n");
       wait(NULL);
       printf("The parent is done\n");
     }

  else
     { printf("The child is about to rewrite itself to run new code\n");
       execvp(*(argv+1), argv+1);
       printf("this prints only if the previous call fails\n");
     }
}

