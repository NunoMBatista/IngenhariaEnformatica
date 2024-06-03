#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <sys/fcntl.h>
#include <semaphore.h>
#include <sys/wait.h>

typedef struct {
	int max_orders;
	int next_order_pos;
} info_struct;

typedef struct {
	pid_t client; //pid client process
  int n_products; //number of products
} order_struct;

typedef struct {
  pid_t worker; //pid worker process
  int sell_value; //value of the sell
} sell_struct;

#define PRODUCT_VALUE 100
