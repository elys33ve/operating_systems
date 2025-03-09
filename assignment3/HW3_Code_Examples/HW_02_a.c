// Program HW_02_a.c 

#include <stdlib.h> 
#include <stdio.h> 
int main(int argc, char **argv) 
   { int arg_count = 0; 
     for (arg_count = 0; arg_count < argc; arg_count++) 
          printf("%s\n", argv[arg_count]); 
   } 


