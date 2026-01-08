/* 
 * Versão de 2ªMeta da disciplina Sistemas Operativos
 * 
 * TRABALHO REALIZADO POR:
 *      Francisco Amado Lapa Marques Silva - 2022213583
 *      Miguel Moital Rodrigues Cabral Martins - 2022213951
 */

#ifndef MESSAGE_QUEUE_STRUCT_C
#define MESSAGE_QUEUE_STRUCT_C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#include "queue_struct.h"

#ifndef REQ_SIZE
#define REQ_SIZE 64
#endif




void create_queue(queue *q, int size, pthread_cond_t *cond, pthread_mutex_t *cond_lock, int *state ,pthread_cond_t *statecond, pthread_mutex_t *statecond_lock) {
    /* Creates a queue on heap */
    q->req_queue = (char (*)[REQ_SIZE])malloc(sizeof(char[REQ_SIZE]) * size);
    q->time_queue = (unsigned long long *)malloc(sizeof(unsigned long long) * size);
    q->size = size;
    q->buf_size = REQ_SIZE;
    pthread_mutex_init(&q->lock, NULL);

    q->writtencond = cond;
    q->writtencond_lock = cond_lock;

    q->state = state;
    q->statecond = statecond;
    q->statecond_lock = statecond_lock;

    q->read_index = 0;
    q->write_index = 0;
    q->count = 0;
    return;
}

/*
void destroy_queue(queue *q) {
    / Frees the queue /
    for (int i = 0; i < q->size; i++) {
        free(q->req_queue[i]);
    }
    free(q->req_queue);
    free(q->time_queue);
    pthread_mutex_destroy(&q->lock);
    free(q);
}
*/


int count_queue(queue *q) {
    /* Returns the number of messages in the queue */
    int count;
    pthread_mutex_lock(&q->lock);
    count = q->count;
    pthread_mutex_unlock(&q->lock);
    return count;
}


int write_queue(queue *q, char *msg) {
    /* Writes a message to the queue
     *      returns 0 if successful, 1 if queue is full
     */
    pthread_mutex_lock(q->writtencond_lock);    // }
    pthread_cond_signal(q->writtencond);        // } Signal the condition variable
    pthread_mutex_unlock(q->writtencond_lock);  // }
    //printf("[HELP] Writing to queue: %s\n", msg);

    if (count_queue(q) == q->size) {
        // queue is full
        pthread_mutex_lock(q->statecond_lock);
        if (*q->state==0) {
            // change state to 1 and signal, to create extra AE
            pthread_cond_signal(q->statecond);          // } Signal state change
            *q->state=1;                                // }
        }
        pthread_mutex_unlock(q->statecond_lock);
        return 1;
    }

    strcpy(q->req_queue[q->write_index], msg);          // }
    q->time_queue[q->write_index] = get_time_millis();  // } Write the message and time

    q->write_index = (q->write_index+1) % q->size;

    pthread_mutex_lock(&q->lock);
    q->count++;
    pthread_mutex_unlock(&q->lock);
    return 0;
}

int read_queue(queue *q, char *msg, unsigned long long *timeout) {
    /* Reads a message from the queue
     * returns 0 if successful, 1 if queue is empty (should never ask to read from an empty queue, but anyway)
     * copies the message to "msg" buffer
     */
    pthread_mutex_lock(q->statecond_lock);
    if (*q->state==1 && count_queue(q)<=q->size/2) {
        // change state to 0 and signal, to destroy extra AE
        pthread_cond_signal(q->statecond);          // } Signal state change
        *q->state=0;                                // }
    }
    if (count_queue(q) == 0) {
        // queue is empty
        return 1;
    }

    strcpy(msg, q->req_queue[q->read_index]);   // }
    *timeout = q->time_queue[q->read_index];    // } Read the message and time

    //printf("[HELP] Reading from queue: %s\n", msg);

    q->read_index = (q->read_index + 1) % q->size;

    pthread_mutex_lock(&q->lock);
    q->count--;
    pthread_mutex_unlock(&q->lock);
    pthread_mutex_unlock(q->statecond_lock);
    return 0;
}

unsigned long long get_time_millis(){
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (unsigned long long)(tv.tv_sec)*1000 + (unsigned long long)(tv.tv_usec)/1000;
}

#endif