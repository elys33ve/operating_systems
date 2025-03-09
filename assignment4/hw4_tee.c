#include <stdlib.h>
#include <stdio.h> 
#include <unistd.h> 
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

#define INPUT_BUFFER 4096

/*  OS ASSIGNMENT 4 - Task 02: Writing Your Own Version of the tee Command
Fiona O'Connell

This program:
    ** supports flags -a, -i
    ** supports multiple output files
    ** uses a pipe
    ** spawns one child process to catch and direct data for output file(s)

gcc hw4_tee.c -o hw4_tee
echo "Hello, world!" | ./hw4_tee file2.txt
cat file1.txt | ./hw4_tee -a file2.txt
cat file1.txt | ./hw4_tee file2.txt file3.txt
*/


int main(int argc, char **argv) {   
    int pipefd[2];
    pid_t pid;
    char c;
    struct sigaction sa;
    char *open_flag = "w";      // flag for write or append file
    int ignore_interrupts = 0;  // track -i option
    int file_count = 0;         // number of output files

    // check arguments
    if (argc < 2) {
        fprintf(stderr, "Usage: %s [-a] [-i] <output_file> ...\n", argv[0]);
        return 1;
    }
    for (int i=1; i<argc; i++) {
        // check for append flag -a
        if (strcmp(argv[i], "-a") == 0) {
            open_flag = "a";
        }
        // check for ignore interrupts flag -i
        else if (strcmp(argv[i], "-i") == 0) {
            ignore_interrupts = 1;
        }
        // count output files
        else {
            file_count++;
        }
    }

    // create pipe
    if (pipe(pipefd)) { 
        perror("pipe");
        return 1;
    }

    // fork child process
    pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {     // -- child process
        close(pipefd[1]);   // close the write end of pipe

        // output files
        int idx = 0;
        FILE *files[file_count];  // array to hold file pointers
        for (int i=1; i<argc; i++) {
            // open output files to write (or append)
            if (strcmp(argv[i], "-a") != 0 && strcmp(argv[i], "-i") != 0) {
                files[idx] = fopen(argv[i], open_flag);
                if (files[idx] == NULL) {
                    perror("fopen");
                    continue;  // skip file if it can't be opened
                }
                idx++;
            }
        }

        // read from pipe and write to files
        char buffer[INPUT_BUFFER];
        ssize_t bytes_read;
        while ((bytes_read = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
            for (int i=0; i<file_count; i++) {
                fwrite(buffer, 1, bytes_read, files[i]);
            }
        }

        // close all files
        for (int i=0; i<file_count; i++) {
            fclose(files[i]);
        }
        if (bytes_read < 0) {
            perror("read");
            return 1;
        }

        close(pipefd[0]);   // close read end of pipe

    } else {      // -- parent process
        close(pipefd[0]);   // close read end of pipe

        // ignore interrupts (flag -i)
        if (ignore_interrupts) {
            sa.sa_handler = SIG_IGN;
            sigemptyset(&sa.sa_mask);
            sa.sa_flags = 0;
            if (sigaction(SIGINT, &sa, NULL) == -1) {
                perror("sigaction");
                return 1;
            }
        }

        // read from standard input and write
        while (read(STDIN_FILENO, &c, 1) > 0) {
            write(STDOUT_FILENO, &c, 1);    // write to standard output
            write(pipefd[1], &c, 1);        // write to the pipe
        }
        
        close(pipefd[1]);   // close write end of pipe
        wait(NULL);         // wait for child process to finish
    }

    return 0;
}
