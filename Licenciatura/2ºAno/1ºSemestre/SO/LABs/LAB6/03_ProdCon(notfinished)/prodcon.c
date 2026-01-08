#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>


#define WAIT_LIST 10
#define CONSUMERS 5
#define PRODUCERS 10
#define SIZE 256
#define MAX_PRODUCED_ITEMS 10

typedef struct c {
	char message [SIZE];
	int entry_number;
} slot;

slot table [WAIT_LIST];
int writepos = 0;
int readpos = 0;
int produced_items=0;
int consumed_items=0;

pthread_mutex_t lock_write = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_read = PTHREAD_MUTEX_INITIALIZER;

// #1
int full = 0;
int empty = WAIT_LIST;
pthread_cond_t lock_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t lock_empty = PTHREAD_COND_INITIALIZER;
// End of #1

void * producer (void * id_ptr);
void * consumer (void *);
void put_data_at (int pos, slot * regist);
void get_data_at (int pos, slot * regist);

int main () {
    int id_p[PRODUCERS];
    int id_c[CONSUMERS];
    pthread_t prods[PRODUCERS];
    pthread_t cons[CONSUMERS];
    int i;
    srand(pthread_self());
    for (i=0; i<PRODUCERS; i++) {
        id_p[i]=i;
        if (pthread_create (&prods[i], NULL, producer, &id_p[i])!=0)
            perror("Error creating producer!");
    }
    for (i=0; i<CONSUMERS; i++) {
        id_c[i]=i;
        if (pthread_create (&cons[i], NULL, consumer, &id_c[i])!=0)
            perror("Error creating consumer!");
    }

    for (i=0; i<PRODUCERS; i++) {
        pthread_join (prods[i], NULL);
        printf("Joined producer %d.\n", i);
    }
    for (i=0; i<CONSUMERS; i++) {
        pthread_join (cons[i], NULL);
        printf("Joined consumer %d.\n", i);
    }

    // #2
    pthread_cond_destroy(&lock_full);
    pthread_cond_destroy(&lock_empty);
    pthread_mutex_destroy(&lock_write);
    pthread_mutex_destroy(&lock_read);
    // End of #2

    printf("\nProgram statistics:\nproduced_items=%d , consumed_items=%d\nwritepos=%d , readpos=%d\n", produced_items, consumed_items, writepos,readpos);
    pthread_exit (NULL);

}

void * producer (void * id_ptr){
    int id = *((int *)id_ptr);
    slot * regist = (slot *)malloc(sizeof(slot));

    int number = 0;
    while (1){
        sleep(rand()%10);
        number = rand()%1000000000;
        regist->entry_number = number; // production registry number (randomly chosen)
        sprintf(regist->message, "service produced by %d", id);

        // #3
        pthread_mutex_lock(&lock_write);
        while (!(empty>0)) {
            if (produced_items>=MAX_PRODUCED_ITEMS) {
                printf("---------------------------------------------------\n");
                pthread_exit(NULL);
            }
            pthread_cond_wait(&lock_full, &lock_write);
        }
        // End of #3

        put_data_at(writepos, regist);
        writepos = (writepos+1)%WAIT_LIST;
        pthread_mutex_unlock(&lock_write);

        // #4
        pthread_mutex_lock(&lock_read);
        empty--;
        pthread_mutex_unlock(&lock_read);
        pthread_mutex_lock(&lock_write);
        full++;
        produced_items++;
        pthread_cond_signal(&lock_empty);
        pthread_mutex_unlock(&lock_write);
        // End of #4
    }
    pthread_exit (NULL);
}

void * consumer (void * id_ptr){
    int id = *((int *)id_ptr);
    int readpos = 0;
    slot regist;
    while (1) {

        // #5
        pthread_mutex_lock(&lock_read);
        while (!(full>0)) {
            if (produced_items>=MAX_PRODUCED_ITEMS) {
                pthread_exit(NULL);
            }
            pthread_cond_wait(&lock_empty, &lock_read);
        }
        // End of #5

        get_data_at(readpos,&regist);
        readpos=(readpos+1)%WAIT_LIST;
        pthread_mutex_unlock (&lock_read);

        // #6
        pthread_mutex_lock(&lock_write);
        full--;
        pthread_mutex_unlock(&lock_write);
        pthread_mutex_lock(&lock_read);
        empty++;
        consumed_items++;
        pthread_cond_signal(&lock_empty);
        pthread_mutex_unlock(&lock_read);
        // End of #6	

        printf("Consumer %d: %d - %s\n", id,regist.entry_number, regist.message);

    }
    pthread_exit (NULL);
}

void put_data_at (int pos, slot * regist) {
    memcpy(&(table[pos]), regist, sizeof(slot));
    fprintf(stderr, "[DEBUG]Insertion n%d id%d \"%s\" at %d.\n", produced_items, regist->entry_number, regist->message, pos);
}

void get_data_at (int pos, slot * regist) {
    memcpy(regist, &(table[pos]), sizeof(slot));
    fprintf(stderr, "[DEBUG]Consumption n%d id%d \"%s\" at %d.\n", consumed_items, regist->entry_number, regist->message, pos);
}
