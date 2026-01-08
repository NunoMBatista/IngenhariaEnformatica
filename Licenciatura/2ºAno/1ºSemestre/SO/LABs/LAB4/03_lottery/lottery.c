#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>
#include <string.h>
#include <sys/ipc.h>

#define SHM_ARRAY_LENGTH 5
#define CONSUMER_AMMOUNT 10

typedef struct shm_struct{
    int readers;
    int numbers[SHM_ARRAY_LENGTH];
} shm_struct;

shm_struct* shared_mem;
int shm_id;
sem_t* mutex,* stop_producer;
pid_t producer_id, consumer_id[CONSUMER_AMMOUNT];


void producer(){
    while(1){
        sem_wait(stop_producer);

        printf("!!!The producer is now writing new numbers!!!\n");
        
        for(int i = 0; i < SHM_ARRAY_LENGTH; i++){
            shared_mem->numbers[i] = 1 + rand() % 48;
        }
        
        sem_post(stop_producer);
        sleep(rand() % 4);
    }
}

void write_to_file(int consumer_number, int number){
    char filename[50]; 
    sprintf(filename, "key_%d.txt", consumer_number);
    FILE* file = fopen(filename, "a"); 
    if(file == NULL){
        perror("Error opening file");
        exit(1);
    }
    fprintf(file, "%d", number);
    fclose(file);
}

void consumer(int consumer_number){
    int visited[50], different_numbers = 0;
    bzero(visited, 50 * sizeof(int));
    while(1){
        sem_wait(mutex);
        shared_mem->readers++;
        if(shared_mem->readers == 1){
            sem_wait(stop_producer);
        }
        sem_post(mutex);

        printf("    Consumer %d is reading\n", consumer_number);

        for(int i = 0; i < SHM_ARRAY_LENGTH; i++){
            if(visited[shared_mem->numbers[i]] == 0 && different_numbers < 6){
                visited[shared_mem->numbers[i]] = 1; 
                different_numbers++;
                write_to_file(consumer_number, shared_mem->numbers[i]);
            }
        }

        sem_wait(mutex);
        shared_mem->readers--;
        if(shared_mem->readers == 0){
            sem_post(stop_producer);
        }
        sem_post(mutex);

        if(different_numbers == 6){
            printf("Consumer %d has found 6 different numbers: ", consumer_number);
            char filename[50];
            sprintf(filename, "key_%d.txt", consumer_number);
            FILE* file = fopen(filename, "r");
            if(file == NULL){
                perror("Error opening file");
                exit(1);
            }

            char key[10];
            fgets(key, 10, file);

            printf("%s\n", key);

            exit(0);
        }

        sleep(1);
    }
}

int main(void){
    shm_id = shmget(IPC_PRIVATE, sizeof(shm_struct), O_CREAT | 0777);
    shared_mem = (shm_struct*) shmat(shm_id, NULL, 0);

    sem_close(mutex);
    sem_unlink("mutex");
    sem_close(stop_producer);
    sem_unlink("stop_producer");

    mutex = sem_open("mutex", O_CREAT | O_EXCL, 0777, 1);
    stop_producer = sem_open("stop_producer", O_CREAT | O_EXCL, 0777, 1);

    // initialize shared memory
    shared_mem->readers = 0;
    bzero(shared_mem->numbers, SHM_ARRAY_LENGTH * sizeof(int));

    if(fork() == 0){
        producer();
        exit(0);
    }
    
    for(int i = 1; i <= CONSUMER_AMMOUNT; ++i){
        if(fork() == 0){
            consumer(i);
            exit(0);
        }
    }

    while(wait(NULL) > 0);

    return 0;
}