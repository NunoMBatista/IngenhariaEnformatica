/*
  Authors: 
    Nuno Batista uc2022216127
    Miguel Castela uc2022212972
*/
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "queue.h"

Queue* create_queue(int max_elements) {
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    queue->num_elements = 0;
    queue->max_elements = max_elements;
    queue->front = queue->rear = NULL;
    return queue;
}

int is_empty(Queue* queue) {
    return queue->num_elements == 0;
}

void push(Queue* queue, Request data) {  // Change this line
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = data;  // Change this line
    newNode->next = NULL;

    if (queue->rear == NULL) {
        queue->front = queue->rear = newNode;
    } else {
        queue->rear->next = newNode;
        queue->rear = newNode;
    }

    queue->num_elements++;
}

Request pop(Queue* queue) {  // Change this line
    if (is_empty(queue)) {
        return failed_request();
    }

    Node* temp = queue->front;
    Request data = temp->data;  // Change this line

    queue->front = queue->front->next;
    if (queue->front == NULL) {
        queue->rear = NULL;
    }

    free(temp);
    queue->num_elements--;

    return data;
}

int is_full(Queue* queue) {
    return queue->num_elements == queue->max_elements;
}

Request failed_request() {
    Request failed;
    failed.request_type = -1;
    return failed;
}

Request front(Queue* queue) {
    if (is_empty(queue)) {
        return failed_request();
    }
    return queue->front->data;
}

Request rear(Queue* queue) {
    if (is_empty(queue)) {
        return failed_request();
    }
    return queue->rear->data;
}
