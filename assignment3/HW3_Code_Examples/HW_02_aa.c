// Program HW_02_a.c 
#include <stdlib.h> 
#include <stdio.h> 

int main(int argc, char **argv) 
    { // argv - a pointer to an array of character pointers
    char **ptr = argv;  // points to the first string in the argv array

    // argc - how many strings are in the argv array
    int arg_count = argc; 

    // loop through the arguments using pointer arithmetic
    while (arg_count > 0) {
        printf("%s\n", *ptr);   // print string pointed to
        ptr++;          // move ptr to next string in the array
        arg_count--;    // decrement the argument count
    }
}