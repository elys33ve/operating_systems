// Program HW_02_h.c

#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

// The following is a function whose purpose is to catch a signal, print out
// a taunting message, then do nothing else.  It takes one parameter, the 
// integer representing the signal caught.

void ignore_signal(int signum)
{ printf("\nI was sent signal number %d, but I choose to ignore it\n", signum);
}

int main(int argc, char **argv, char **envp)
{ int c;

  // This system call "registers" a function to be the programatic entity
  // that is to be run whenever the process containing this code is
  // given the referenced signal number by the kernel.  In this case,
  // we're saying that whenever the process gets the SIGINT signal 
  // (see the wikipedia page) it should run the function pointed to
  // by the value "ignore_signal" -- which happens to be the code for the 
  // handler function.  SIGINT is the "nice interrupt".  The default handler
  // would gracefully wrap up the process and then terminate.  The 
  // replaced handler taunts and ignores the entity requesting the 
  // interrupt.

  signal(SIGINT, ignore_signal);

  // Look at that...  a big old busy wait loop.  Note that we never
  // directly call the handler.  The handler gets called via the action
  // of the kernel when the signal "arrives".  It can run at any time 
  // after it arrives and between non-atomic operations of this process.
  // Assuming the handler didn't exit the process, the process will return
  // to whatever it had already been doing once the handler is done running.
  //
  // Compile and run this code from the command shell.  Hit control-c a few
  // times.  What happens?

  for (c=0; c < 1000000000; c++);


  exit (0);
}

  
