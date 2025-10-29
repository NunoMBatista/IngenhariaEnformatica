// Operating Systems FCTUC - 2021
// Compile as: gcc -Wall -pthread factory_main.c -o factory_main
//
// Not everything in the code is needed; some parts are used only to provide some knowledge about pointer arithmetic

#include <signal.h>
#include "factory.h"

int shmid, n_orders;
sem_t *go_main, *go_workers, *mutex;
key_t shmkey;
info_struct *info;
order_struct *orders;
sell_struct *sells;

void show_shm(){
  printf("Showing shm (integer by integer):\n");
  // The effect of p+n where p is a pointer and n is an integer is to compute the address equal to p plus n times the size of whatever p points to
  int *p = (int *) info;
  for(int i=0;i<( (sizeof(info_struct)+sizeof(order_struct)*n_orders+sizeof(sell_struct)*n_orders)/sizeof(int));i++){
    printf("%d  ",*(p+i));
    if(i%2==1) printf("\n");
  }
  printf("\n");
}

void finish() {
  show_shm();

  printf("Cleaning resources...\n");
  sem_close(mutex);
  sem_close(go_workers);
  sem_close(go_main);
  sem_unlink("MUTEX");
  sem_unlink("GO_WORKERS");
  sem_unlink("GO_MAIN");
  shmdt(info);
  shmctl(shmid, IPC_RMID, NULL);

  printf("Closing program!\n");
  exit(0);
}

void sigint(int signum){
  finish();
}

int main(int argc, char* argv[]) {
	signal(SIGINT, sigint);

	if (argc!=2){
		printf("Invalid number of arguments!\nUse as: factory_main {maximum number of orders}\n");
		exit(1);
	}

  n_orders=atoi(argv[1]);
  if(n_orders<=0){
    printf("Wrong argument!\n");
    exit(1);
  }

	if( (shmkey = ftok(".", getpid())) == (key_t) -1){
    perror("IPC error: ftok");
    exit(1);
  };
	printf("Created SHM KEY = %d\n",shmkey);

	shmid = shmget(shmkey, sizeof(info_struct)+sizeof(order_struct)*n_orders+sizeof(sell_struct)*n_orders, IPC_CREAT|IPC_EXCL|0700); // shm memory cannot exist
	if (shmid < 1){
		perror("Error creating shm memory!\n");
		exit(1);
	}

	info = (info_struct*) shmat(shmid, NULL, 0);
	if (info < (info_struct*) 1){
		perror("Error attaching memory!\n");
    finish();
		exit(1);
	}

	sem_unlink("MUTEX");
	mutex=sem_open("MUTEX",O_CREAT|O_EXCL,0700,1); // create semaphore
	if(mutex==SEM_FAILED){
		perror("Failure creating the semaphore MUTEX");
    finish();
    exit(1);
	}
	sem_unlink("GO_WORKERS");
	go_workers=sem_open("GO_WORKERS",O_CREAT|O_EXCL,0700,0); // create semaphore
	if(go_workers==SEM_FAILED){
		perror("Failure creating the semaphore GO_WORKERS");
    finish();
    exit(1);
	}
  sem_unlink("GO_MAIN");
	go_main=sem_open("GO_MAIN",O_CREAT|O_EXCL,0700,0); // create semaphore
	if(go_main==SEM_FAILED){
		perror("Failure creating the semaphore GO_MAIN");
    finish();
    exit(1);
	}

  //initialize info in shm
  info->max_orders=n_orders;
	info->next_order_pos=0; //the first order position free is 0

  printf("Waiting for clients to place orders and workers to finish them!\n");
  sem_wait(go_main);

	finish();
}
