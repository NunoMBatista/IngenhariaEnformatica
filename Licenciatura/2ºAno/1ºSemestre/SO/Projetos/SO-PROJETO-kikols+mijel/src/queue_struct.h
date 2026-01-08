/* 
 * Versão de 2ªMeta da disciplina Sistemas Operativos
 * 
 * TRABALHO REALIZADO POR:
 *      Francisco Amado Lapa Marques Silva - 2022213583
 *      Miguel Moital Rodrigues Cabral Martins - 2022213951
 */

#ifndef MESSAGE_QUEUE_STRUCT_H
#define MESSAGE_QUEUE_STRUCT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#include <pthread.h>

#ifndef REQ_SIZE
#define REQ_SIZE 64
#endif

typedef struct queue {
    char (*req_queue)[REQ_SIZE];
    unsigned long long *time_queue;
    int size;
    int buf_size;
    pthread_mutex_t lock;
    pthread_cond_t *writtencond;        // } pointer to the condition variable that signals written that will be used
    pthread_mutex_t *writtencond_lock;  // } to signal the condition

    int *state;                             // } pointer to the state of the queue (0-normal, 1-need extra AE)
    pthread_cond_t *statecond;               // } pointer to the condition variable that signals half-full that will be used
    pthread_mutex_t *statecond_lock;         // } to signal the condition

    int read_index;
    int write_index;
    int count;          // Number of messages in the queue
} queue;


void create_queue(queue *q, int size, pthread_cond_t *cond, pthread_mutex_t *cond_lock, int *state, pthread_cond_t *statecond, pthread_mutex_t *statecond_lock);
// void destroy_queue(queue *q);

int count_queue(queue *q);

int write_queue(queue *q, char *msg);
int read_queue(queue *q, char *msg, unsigned long long *time);

/* UTILS */
unsigned long long get_time_millis();

#endif