// Compile with: gcc -Wall two_children.c -o twoc
// Complete the code bellow

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>


int child_1(){
  sleep(5);
	int exit_value;
	printf("Child 1 (PID=%d): starting!\n",getpid());
	exit_value=rand()%10;
	printf("Child 1: Result from execution is going to be %d!\n",exit_value);
	printf("Child 1: ready to exit!\n");
	return(exit_value);
}

void child_2(){
	printf("Child 2 (PID=%d): going to execute 'ps' in two seconds!\n",getpid());
	sleep(2);
	printf("Child 2: Checking processes with ps! Any zombies here?!\n");
	execlp("ps","ps",NULL);
	// if everything goes well this code will be substituted by "ls" code
}

int main(void) {
  pid_t child_pid_1, child_pid_2;
  int return_status;

  printf("Father process (PID=%d): going to create 2 new processes!\n", getpid());

  child_pid_1 = fork();
  if (child_pid_1 == 0) {
    exit(child_1());
  }

  child_pid_2 = fork();
  if (child_pid_2 == 0) {
    child_2();
    exit(1); //if this exit is executed some error ocurred with exec!
  }


  printf("Father process: New processes created! Waiting for Child 2!\n");
  // insert code here
  waitpid(child_pid_2, &return_status, 0); 


	// end of insert
  printf("Father process: Child 2 finished! Now waiting for Child 1 and its returning code!\n");
  // insert code here
  waitpid(child_pid_1, &return_status, 0);


	// end of insert
  printf("Father process: Child 1 finished with status %d\n",WEXITSTATUS(return_status));
  return 0;
  }
