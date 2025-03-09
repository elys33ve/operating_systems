// Program HW_02_b.c

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv)
   { char *env_variable_ptr;
     env_variable_ptr = getenv("PATH");
     printf("The setting of the shell variable PATH is: %s\n", env_variable_ptr);

   }

