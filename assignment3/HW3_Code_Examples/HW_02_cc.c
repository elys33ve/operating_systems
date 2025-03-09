// Program HW_02_c.c
#include <stdlib.h>
#include <stdio.h>

// extern, applied to a C global variable, indicates it's defined elsewhere, like the standard C library

int main(int argc, char **argv, char **envp)
   { // envp is a pointer to an array of character pointers - environmental variables
    char **env_variable_ptr = envp;
     
    // look through the environmental variables
    while (*env_variable_ptr != NULL)
        { printf("%s\n", *env_variable_ptr);    // print environmental variable string
          env_variable_ptr++;       // move pointer
        }
     
    printf("\n");
   }