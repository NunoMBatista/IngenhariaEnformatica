//compile with: gcc -Wall -pthread sharedvariable_posix.c -o svar
//using POSIX named semaphores

#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h> // include POSIX semaphores

/*insert here the other libraries needed*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

int * shared_var;
int shmid;
sem_t *mutex;

void worker(){
	usleep(1000000 + rand()%11*100000); //uses microseconds sleep and waits 1 to 2 seconds in periods of 0.1 secs
	//sleep(rand() % 10);
	/*insert code here*/
	printf("Worker process [%d]\n", getpid());
	sem_wait(mutex);
	*shared_var += 1;
	sem_post(mutex);
	exit(0);
}

int main(int argc, char *argv[]){
	int i, n_procs;

	if(argc!=2) {
		printf("Wrong number of parameters\n");
		exit(0);
		}

	n_procs=atoi(argv[1]);

	// Create shared memory
	shmid = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0777);
	if(shmid == -1){
		perror("shmget");
		exit(1);
	}

	// Attach shared memory
	shared_var = (int *) shmat(shmid, NULL, 0);
	if(shared_var == (int *) -1){
		perror("shmat");
		exit(1);
	}

	
	// Check if the semaphore already exists and unlink it
	sem_close(mutex);
	sem_unlink("mutex");
	// Initialize semaphore with value 1
	mutex = sem_open("mutex", O_CREAT | O_EXCL, 0777, 1);	
	if(mutex == SEM_FAILED){
		perror("sem_open");
		exit(1);
	}

	// initialize variable in shared memory
	/*insert code here*/
	*shared_var = 1000;

	// Create worker processes
	/*insert code here*/
	for(i = 0; i < n_procs; i++){
		if(fork() == 0){
			worker();
		}
	}
	
	// Wait for all worker processes
	/*insert code here*/
	while(wait(NULL) > 0);

	// print final result
	/*insert code here*/
	printf("Final value: %d\n", *shared_var);

	// remove resources (shared memory and semaphore)
	/*insert code here*/	
	shmctl(shmid, IPC_RMID, NULL);
	sem_close(mutex);
	sem_unlink("mutex");

	exit(0);
}
