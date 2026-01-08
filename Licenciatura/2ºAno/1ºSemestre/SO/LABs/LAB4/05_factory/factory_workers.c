// Operating Systems FCTUC - 2021
// Compile as: gcc -Wall -pthread factory_workers.c -o factory_workers

#include "factory.h"

int shmid, n_workers, n_orders;
sem_t *go_main, *go_workers, *mutex;
key_t shmkey;
info_struct *info;
order_struct *orders;
sell_struct *sells;


void worker() {
  int order_number;

	printf("[Worker %d] Hello!\n", getpid());

  while(1){
	  sem_wait(mutex);
	  if(info->next_order_pos==info->max_orders){
			sem_post(mutex);
			break;
		}
		order_number=info->next_order_pos;
		info->next_order_pos=info->next_order_pos+1;
		sem_post(mutex);

	  printf("[Worker %d] Will make order %d!\n", getpid(),order_number);
		sleep(1);
		sells[order_number].worker=getpid();
		sells[order_number].sell_value=orders[order_number].n_products*PRODUCT_VALUE;
		printf("[Worker %d] Order %d finished!\n", getpid(),order_number);
  }
	printf("[Worker %d] Bye!\n", getpid());
}

int main(int argc, char* argv[]) {
	if (argc!=3){
		printf("Invalid number of arguments!\nUse as: factory_workers {number of workers} {shm key}\n");
		exit(1);
	}

  n_workers=atoi(argv[1]); //number of clients to create
	if(n_workers<1) {
		printf("Invalid number of workers argument!\n");
		exit(1);
	}
	shmkey=atoi(argv[2]); // shm key

  shmid = shmget(shmkey,0,0700); // opens existing shm
	if (shmid < 1){
		perror("Error opening shm memory!\n");
		exit(1);
	}

	info = (info_struct*) shmat(shmid, NULL, 0);
	if (info < (info_struct*) 1){
		perror("Error attaching memory!\n");
		exit(1);
	}

  n_orders=info->max_orders;
  printf("Maximum number of orders: %d\n",n_orders);

  orders = (order_struct *)(info + 1);
  sells = (sell_struct *) (orders + n_orders);

	mutex=sem_open("MUTEX",0,0700,1); //open existing semaphore
  if(mutex==SEM_FAILED){
		perror("Failure opening the semaphore MUTEX");
		exit(1);
	}
	go_workers=sem_open("GO_WORKERS",0,0700,1); //open existing semaphore
  if(go_workers==SEM_FAILED){
		perror("Failure opening the semaphore GO_WORKERS");
		exit(1);
	}
	go_main=sem_open("GO_MAIN",0,0700,1); //open existing semaphore
	if(go_main==SEM_FAILED){
		perror("Failure creating the semaphore GO_MAIN");
		exit(1);
	}

  printf("Waiting for clients to place orders!\n");
	sem_wait(go_workers); // wait for clients to finish placing orders

	//Creating workers
  for(int i=0;i<n_workers;i++){
		if (fork() == 0) {
	    worker();
	    exit(0);
		}
	}

  for(int i=0;i<n_workers;i++){
		wait(NULL);
	}

	printf("Go main!\n");
	sem_post(go_main);
	printf("Closing program!\n");
}
