#ifndef MESSAGE_QUEUE_STRUCT_C
#define MESSAGE_QUEUE_STRUCT_C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "queue_struct.h"

#ifndef BUF_SIZE
#define BUF_SIZE 1024
#endif




void create_queue(queue *q, int size, int buf_size, pthread_cond_t *cond, pthread_mutex_t *cond_lock) {
    /* Creates a queue on heap */
    q->queue = (char **)malloc(sizeof(char*) * size);
    for (int i = 0; i < size; i++) {
        q->queue[i] = (char *)malloc(sizeof(char) * buf_size);
    }
    q->size = size;
    q->buf_size = buf_size;
    pthread_mutex_init(&q->lock, NULL);
    q->cond = cond;
    q->cond_lock = cond_lock;
    q->read_index = 0;
    q->write_index = 0;
    q->count = 0;
    return;
}

/*
void destroy_queue(queue *q) {
    / Frees the queue /
    for (int i = 0; i < q->size; i++) {
        free(q->queue[i]);
    }
    free(q->queue);
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
     * returns 0 if successful, 1 if queue is full
     */
    pthread_mutex_lock(&q->lock);
    if (q->count == q->size) {
        pthread_mutex_unlock(&q->lock);
        return 1;
    }
    strcpy(q->queue[q->write_index], msg);
    q->write_index = (q->write_index + 1) % q->size;
    q->count++;

    pthread_mutex_lock(q->cond_lock);   // }
    pthread_cond_signal(q->cond);       // } Signal the condition variable
    pthread_mutex_unlock(q->cond_lock); // }

    pthread_mutex_unlock(&q->lock);
    return 0;
}

int read_queue(queue *q, char *msg) {
    /* Reads a message from the queue
     * returns 0 if successful, 1 if queue is empty (should never ask to read from an empty queue, but anyway)
     * copies the message to "msg" buffer
     */
    pthread_mutex_lock(&q->lock);
    if (q->count == 0) {
        pthread_mutex_unlock(&q->lock);
        return 1;
    }
    strcpy(msg, q->queue[q->read_index]);
    q->read_index = (q->read_index + 1) % q->size;
    q->count--;
    pthread_mutex_unlock(&q->lock);
    return 0;
}

#endif