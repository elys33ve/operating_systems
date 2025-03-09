#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

/*  HW3 Task 2: Write a Simple Command Shell
Fiona O'Connell 

Built-in commands:
-- printenv - prints ALL environment variables of the current shell.
-- pwd - print working directory
-- cd - change directory

LAUNCH child processes named on via commands to the shell. These processes 
must exec() the name of the program to be launched according to the current 
setting of the PATH environment variable.
    - The named process must run with any and all command line parameters you passed it
    - YOUR SHELL SHOULD IGNORE ctrl-c. 
        Typing ctrl-c at a shell prompt should have no effect.  
        Typing ctrl-c while a built-command is running should have no effect. 
    - YOUR SHELL SHOULD IGNORE control-z. 
        Typing ctrl-z at a shell prompt should have no effect. 
        Typing ctrl-z while a built-in command is running should have no effect.
    - When a non-built-in command is running, ctrl-c should terminate the process.  
        When a command terminates normally, the shell should print a message that says: 
        <PID> terminated normally where <PID> is the process ID of the command that just exited. 
    - When a non-built-in command is running, ctrl-z should stop/pause the process. 
        When a command is paused, the shell should print a message that says: 
        <PID> is stopped. 
        Note that when a process is stopped, it IS possible to restart it by sending it a SIGCONT 
        command from this, or another shell, to that PID. You should test that you can restart 
        stopped processes in this way either from the sillyshell that launched it or from another 
        shell running in another window.

*/

#define LINE_BUFFER 1024


pid_t wpid = 0;  // foreground process being waited on

/**************************************************************/
/* Text Processing / Parsing Routines                         */
/**************************************************************/

void  parse(char *line, char **argv) {
    // We will assume that the input string is NULL terminated.  If it
    // is not, this code WILL break.  The rewriting of whitespace characters
    // and the updating of pointers in argv are interleaved.  Basically
    // we do a while loop that will go until we run out of characters in
    // the string (the outer while loop that goes until '\0').  Inside
    // that loop, we interleave between rewriting white space (space, tab,
    // and newline) with nulls ('\0') AND just skipping over non-whitespace.
    // Note that whenever we encounter a non-whitespace character, we record
    // that address in the array of address at argv and increment it.  When
    // we run out of tokens in the string, we make the last entry in the array
    // at argv NULL.  This marks the end of pointers to tokens.  Easy, right?

    
    while (*line != '\0') { 
        // outer loop. keep going until the whole string is read.
        // keep moving the pointer forward into the input string until
        // we encounter a non-whitespace character. While we're at it,
        // turn all those whitespace characters we're seeing into null chars.
        while (*line == ' ' || *line == '\t' || *line == '\n' || *line == '\r') { 
            *line = '\0';     
            line++;
        }

        // If I got this far, I MUST be looking at a non-whitespace character,
        // or, the beginning of a token.  So, let's record the address of this
        // beginning of token to the address I'm pointing at now. (Put it in *argv)
        // then we'll increment argv so that the next time I store an address, it 
        // will be in the next slot of the array of integers.

        *argv++ = line;          /* save the argument position     */

        // Ok... now let's just keep incrementing the input line pointer until
        // I'm looking at whitespace again.  This "eats" the token I just found
        // and sets me up to look for the next.

        while (*line != '\0' && *line != ' ' && 
                *line != '\t' && *line != '\n' && *line !='\r') {
            line++;             /* skip the argument until ...    */
        }
    }

    // Heh, I ran out of characters in the input string.  I guess I'm out of tokens.
    // So, whatever slot of the array at argv I'm pointing at?  Yeah, put a NULL
    // there so we can mark the end of entries in the table.

    *argv = NULL;                 /* mark the end of argument list  */
}


/**************************************************************/
/* Signal Handling Routines                                   */
/**************************************************************/

// --- signal handlers
void sigint_handler(int sig) {
    /*  ctrl-c  */
    if (wpid > 0) {   // if a foreground process exists, terminate it
        kill(wpid, SIGINT);
    }
}
void sigtstp_handler(int sig) {
    /*  ctrl-z  */
    if (wpid > 0) {   // pause foreground process
        kill(wpid, SIGTSTP);
        printf("\nProcess %d is stopped.\n", wpid);
    }
}

void ignore_signal(int signum) {}

/**************************************************************/
/* Shell Command Processing Routines                          */
/**************************************************************/

void execute(char **argv, char *raw_line_input) {
    pid_t pid;
    int status;
    
    if ((pid = fork()) < 0) {   // fork child process
        printf("*** ERROR: forking child process failed\n");
        exit(1);
    } else if (pid == 0) { 
        /* for child process: execute command */
        signal(SIGINT, SIG_DFL);  // restore default ctrl-c behavior
        signal(SIGTSTP, SIG_DFL); // restore default ctrl-z behavior

        if (execvp(argv[0], argv) < 0) { 
            printf("*** ERROR: failed to execute (%s)\n", raw_line_input);
            exit(1);
        }
        //printf("The command line <%s> should have been executed here\n", raw_line_input);
        exit (0); 
    } else { 
        /* for parent process: wait and track child process */
        wpid = pid;
        waitpid(pid, &status, WUNTRACED);   // wait for termination or stop

        if (WIFEXITED(status)) {
            printf("Process %d terminated normally.\n", pid);
        } else if (WIFSTOPPED(status)) {
            printf("Process %d is stopped.\n", pid);
        }
        wpid = 0;  // reset foreground process tracking
        return;
    }
    return;
}


// --- handle child processes with non-blocking wait
void check_child_processes() {
    pid_t pid;
    int status;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        if (WIFEXITED(status)) {
            printf("Process %d terminated with exit status %d\n", pid, WEXITSTATUS(status));
        }
    }
}


int main(int argc, char **argv, char **envp) {    
    int status;
    int index = 0;

    char line[LINE_BUFFER];     // string buffer to hold the string typed in by the user. 
                                // The str will be parsed, the shell will do what it needs 
                                // to do based on the tokens it finds

    char raw_line[LINE_BUFFER]; // Holds an unmodified copy of the user's input at the command line.
                                        
    char *line_argv[64];    // A pointer to an array of 64 pointers to char, or, an 
                            // array of pointers to strings. After parsing, this array 
                            // will hold pointers to memory INSIDE the string pointed to by 
                            // the pointer line

    char shell_prompt[15];  // This string will hold the shell prompt string
    
    // set the default prompt
    strcpy(shell_prompt, "SillyShell");

    // Signal handlers: ignore ctrl-c and ctrl-z in shell itself
    signal(SIGINT, sigint_handler);
    signal(SIGTSTP, sigtstp_handler);

    while (1) {  
        printf("%s> ", shell_prompt);  // display the shell prompt

        if(fgets(line, 1024, stdin) == NULL) { 
            printf("\n");
            exit(0);
        };  
             
        line[strlen(line)-1]='\0';      // Replace the newline character with null terminator 
        strncpy(raw_line, line, 1024);

        // check if there's status signals coming from children
        check_child_processes();

        // If something was actually typed, then do something...
        if ((*line != '\0') && (*line > 31)) { 
            
            // First, get all the addresses of all of the tokens inside the input line
            parse(line, line_argv);     // parse the line to break it into token references

            // Check the first token for built in commands to handle directly.
            // if we hit the end of the ladder, assume the command line was requesting
            // an external program be run as a child process
            if (strcmp(line_argv[0], "exit") == 0) { 
                exit(0); 
            } else if (strcmp(line_argv[0], "done") == 0) { 
                exit(0); 
            } else if (strcmp(line_argv[0], "quit") == 0) { 
                exit(0); 
            } else if (strcmp(line_argv[0], "printenv")  == 0) {    // print environmental variables
                for (char **env = envp; *env != NULL; env++) {
                    printf("%s\n", *env);
                }
            } else if (strcmp(line_argv[0], "pwd") == 0) {         // print working dir
                char cwd[256];
                if (getcwd(cwd, sizeof(cwd)) != NULL) {
                    printf("%s\n", cwd);
                } else {
                    perror("getcwd() error");
                }
            } else if (strcmp(line_argv[0], "cd") == 0) {          // change directory
                if (line_argv[1] == NULL) {
                    chdir(getenv("HOME"));  // change to home directory if no argument
                } else {
                    if (chdir(line_argv[1]) != 0) {
                        perror("chdir() error");
                    }
                }
            } else if (strcmp(line_argv[0], "newprompt") == 0) { 
                if (line_argv[1] != NULL) {
                    strncpy(shell_prompt, line_argv[1], 15); 
                } else {
                    strncpy(shell_prompt, "SillyShell", 15);
                }
            } else {                                                                       
                // Not a built-in. Process the line as if the first token is the name of a program 
                // somewhere in the current path environment variable
                execute(line_argv, raw_line);
            }
               
        }
    }
}

                

