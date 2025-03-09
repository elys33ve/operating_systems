
// Program HW_02_i.c

#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void SIGINT_signal(int signum)
{ printf("\nI told myself to stop with a SIGINT I sent myself.  Bye Bye\n");
  exit (0);
}

int main(int argc, char **argv, char **envp)
{ int c;

  signal(SIGINT, SIGINT_signal);

  for (c=0; c < 1000000000; c++)
      { // The raise system call will send the indicated signal 
        // to the process from which it is run
        if (c == 5000) raise(SIGINT);
      }

  printf("I should never make it here...\n");

  exit (0);
}

  
