#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv) { 
    int num_children;
    pid_t pid;

    // prompt user for number of children
    printf("How many children should I make? ");
    scanf("%d", &num_children);

    // check for errors in input
    if (num_children < 0) {
        fprintf(stderr, "Invalid number of children.\n");
        return 1;
    }

    // loop to create children
    for (int i=0; i<num_children; i++) {
        pid = fork();   // create new child process
        if (pid < 0) {  // check for error
            fprintf(stderr, "fork() failed.\n");
            return 1;
        }

        if (pid == 0) {
            printf("Hello from child process %d\n", getpid());  // print message with child PID
            exit(0);    // child terminates
        }
    }

    // parent waits for children to finish
    for (int i=0; i<num_children; i++) {
        wait(NULL); 
    }

    printf("Hello from parent process %d\n", getpid()); // print message with parent PID

    return 0;
}
