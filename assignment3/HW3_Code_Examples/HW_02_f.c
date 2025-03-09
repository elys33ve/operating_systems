// Program HW_02_f.c
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char **argv) { 
   pid_t fork_returned_pid;
   int count;
   int return_status;

   fork_returned_pid = fork();

   if (fork_returned_pid < 0) { 
      printf("The fork() didn't work! Terminate\n");
      exit (0); 
   }

   if (fork_returned_pid != 0) { 
      wait(&return_status);   // This line is new
      if (WIFEXITED(return_status))
         printf("\nChild Terminated Normally with a return value of %d\n", WEXITSTATUS(return_status));
   }

   for (count = 0; count < 10; count++) { 
      if (fork_returned_pid == 0)
            printf("                              child says:  count = %d\n", count);
         else
            printf("parent says: count = %d\n", count);
      }

   printf("\n");

   if (fork_returned_pid == 0)
      exit(10);
   else
      exit(20);
}

