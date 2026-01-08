#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(void) {
  pid_t mypid;
  pid_t childpid;

  mypid = getpid();
  childpid = fork();

  if (childpid == 0) {
    printf("child mypid :%ld\n", (long)mypid);
    printf("child getpid() :%ld\n", (long)getpid());
    printf("child getppid() :%ld\n", (long)getppid());
  } else {
    sleep(10);
    printf("mypid :%ld\n", (long)mypid);
    printf("getpid() :%ld\n", (long)getpid());
    printf("getppid() :%ld\n", (long)getppid());
  }
  return 0;
}

/* 
a) 
getpid() returns the process ID of the calling process.
getppid() returns the parent process ID of the calling process.

b)
Not explicitly, but the father must finish first, because the child is adopted by the init process.

c)
Now the father is sure to finish first, and the child will be adopted by the init process.

d)
The father of the father process is the bash process.

e)
Now the child finishes first.


*/