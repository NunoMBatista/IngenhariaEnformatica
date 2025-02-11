#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(void) {
  pid_t mypid;
  pid_t childpid;

  mypid = getpid(); // Get father's process ID
  childpid = fork(); // Generate child

  // Block of code executed by the child
  if (childpid == 0) { 
    printf("child mypid :%ld\n", (long)mypid); // Prints father's process id 
    printf("child getpid() :%ld\n", (long)getpid()); // Prints child's process id
    printf("child getppid() :%ld\n", (long)getppid()); // Prints child's prespective of father's process id after he dies (1)
  } 
  // Block of code executed by the father
  else {
    sleep(10);
    printf("mypid :%ld\n", (long)mypid); // Print's father's process ID
    printf("getpid() :%ld\n", (long)getpid()); // Print's father's process ID
    printf("getppid() :%ld\n", (long)getppid()); // Print's father's father process ID (the bash process ID)
  }
  return 0;
}

/* 
a) mypid has the original father's process ID from the beggining, 
  so it always prints the same id, getpid() gets the current process' ID,
  that's why mypid = getpid() in the father's code block.

b) The father finishes first.

c) The father dies first and the child becomes an orphan, defaulting
  it's new father to init (process ID = 1)

d) The bash process.

e) Now the child executes and finished first
*/