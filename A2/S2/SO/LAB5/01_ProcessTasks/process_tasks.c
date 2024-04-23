// compile as gcc -pthread -Wall process_tasks.c -o ptasks

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define N 10

int processing_list[N];
int next_request=0;
pthread_t my_thread[N];

void *worker(void* p) {
    int w_id=*((int *)p);
    pthread_t id=pthread_self();

    printf("Thread %ld: Worker %d starting\n",id,w_id);
    printf("Thread %ld: Worker %d is going to execute order %d\n",id, w_id, next_request);
    processing_list[next_request]=w_id; //save the worker that took the order
    sched_yield(); // force thread to yield the processor - just to simulate processor scheduling
    next_request++; //update next request number
    usleep(10); // simulates the processing of the order
    printf("Thread %ld: Worker %d finishing\n",id,w_id);
    pthread_exit(NULL);
}

int main(void) {
    int i, worker_id[N];
    
    for (i = 0; i < N; i++) processing_list[i]=-1; //initialize processing_list array 
    
    // create N threads
    for (i = 0; i < N; i++) {
        worker_id[i]=i+1;
        pthread_create(&my_thread[i], NULL, worker, &worker_id[i]);
    }

    for (i=0; i<N; i++) {
        pthread_join(my_thread[i], NULL);
    }


    printf("Final list -----\n");
    for (i = 0; i < N; i++) printf(" %d => worker %d ;\n",i,processing_list[i]);
    exit(0);
}
