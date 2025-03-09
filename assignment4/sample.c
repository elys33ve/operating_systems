#include <stdlib.h>
#include <stdio.h> 
#include <unistd.h> 
#define MSGSIZE 6

/*
sample: simplified version of the Linux/Unix cat command.
*/


int main(int argc, char **argv) {   
    char c;
    FILE *fp;
    
    // get file from arg and read
    if (argc > 1) { 
        fp = fopen(argv[1], "r");

        if (fp == NULL) { 
            fprintf(stderr, "ERROR - FILE %s NOT FOUND\n", argv[1]);
            exit(1);
        } 
    } else {
        fp = stdin;
    }
    
    // read and output
    while (!feof(fp)) { 
        c = getc(fp);

        if (!feof(fp)) {
            putc(c, stdout);
        }
    }
}
