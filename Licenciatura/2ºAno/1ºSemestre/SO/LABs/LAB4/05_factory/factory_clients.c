// Operating Systems FCTUC - 2021
// Compile as: gcc -Wall -pthread factory_clients.c -o factory_clients

#include "factory.h"

int shmid, n_clients, n_orders;
sem_t *go_workers, *mutex;
key_t shmkey;
info_struct *info;
order_struct *orders;


void client() {
  int order_number;
  srand(getpid());

	printf("[Client %d] Hello!\n", getpid());

  while(1){
	  sem_wait(mutex);
	  if(info->next_order_pos==info->max_orders){
			sem_post(mutex);
			break;
		}
		order_number=info->next_order_pos;
		info->next_order_pos=info->next_order_pos+1;
		sem_post(mutex);

	  printf("[Client %d] Put order %d!\n", getpid(),order_number);
		orders[order_number].client=getpid();
		orders[order_number].n_products=rand()%10+1;

		sleep(1);
  }
	printf("[Client %d] Bye!\n", getpid());
}

int main(int argc, char* argv[]) {
	if (argc!=3){
		printf("Invalid number of arguments!\nUse as: factory_clients {number of clients} {shm key}\n");
		exit(1);
	}

  n_clients=atoi(argv[1]); //number of clients to create
  if(n_clients<1) {
    printf("Invalid clients argument!\n");
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

	//Creating clients
  for(int i=0;i<n_clients;i++){
		if (fork() == 0) {
	    client();
	    exit(0);
		}
	}

  for(int i=0;i<n_clients;i++){
		wait(NULL);
	}

  info->next_order_pos=0; //put order_pos back to 0
  printf("Go workers!\n");
  sem_post(go_workers); // put workers to work!
  printf("Closing program!\n");
}
