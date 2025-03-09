// Program HW_02_c.c

#include <stdlib.h>
#include <stdio.h>

extern char **environ; // look into what “extern” means when applied to 
                       // a C global variable :)


int main(int argc, char **argv)
   { char **env_variable_ptr;
     
     env_variable_ptr = environ;
     
     while (*env_variable_ptr != NULL)
        { printf("%s\n", *env_variable_ptr);
          env_variable_ptr++;
        }
     
     printf("\n");

   }

