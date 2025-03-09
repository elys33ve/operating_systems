// Program HW3_Task6.c
// Fiona O'Connell
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#define NUM_CHILDREN 5

pid_t children[NUM_CHILDREN];  // array to store child PIDs


// --- signal handler for SIGINT
void sigint_handler(int sig) {
    static int count = 0;   // count for number of signals received
    count++;

    // print for whether signal is ignored or honored
    if (count == 1) {
        printf("Child PID %d: received SIGINT - ignored\n", getpid());
    } else {
        printf("Child PID %d: received SIGINT - terminating\n", getpid());
        exit(0);    // exit on second SIGINT
    }
}

// --- countdown print
void countdown(int s) {
    for (int i=0; i<s; i++) {
        printf("Countdown %d\n", s-i);
        sleep(1);
    }
}

int main() {
    pid_t pid;

    // create child processes & check for errors
    printf("Parent: creating %d children\n", NUM_CHILDREN);
    for (int i=0; i<NUM_CHILDREN; i++) {
        pid = fork();
        if (pid < 0) {
            fprintf(stderr, "fork() failed");
            exit(1);
        }

        // child processes:
        if (pid == 0) {
            signal(SIGINT, sigint_handler);
            while (1) {
                sleep(1);   // infinite loop
            }
        }
        children[i] = pid;  // store child PID in array
    }

    // parent process:
    countdown(5);   // wait for 5 seconds

    // send SIGINT to all children (first round is ignored)
    printf("Parent: sending first SIGINT to all children.\n");
    for (int i=0; i<NUM_CHILDREN; i++) {
        kill(children[i], SIGINT);
    }

    countdown(5);   // wait for 5 seconds again

    // send SIGINT to all children (second round is honored)
    printf("Parent: sending second SIGINT to all children.\n");
    for (int i=0; i<NUM_CHILDREN; i++) {
        kill(children[i], SIGINT);
    }

    // wait for child processes to terminate
    for (int i=0; i<5; i++) {
        wait(NULL);
    }

    printf("Parent %d: all children have terminated.\n", getpid());

    return 0;
}

/*  Informal bonus question: WHY did the children print their messages AFTER the second countdown has started?  

The children's messages display on the terminal after the parent already moved on to the next part because the parent
moves onto the second countdown immediately after sending the signals, but receiving the signals doesn't instantly print
the signal output.
*/
