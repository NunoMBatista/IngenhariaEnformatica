/*
  Authors: 
    Nuno Batista uc2022216127
    Miguel Castela uc2022212972
*/
#ifndef QUEUE_H
#define QUEUE_H

#include "global.h"

typedef struct{
    int user_id; // ID of the user that made the request 
    /*
        Request types:
        I - Register user
        V - Video
        M - Music
        S - Social
        D - Backoffice data_stats
        R - Backoffice reset
        E - Backoffice invalid request
        F - Failed request
        K - Kill auth engine
    */
    char request_type;
    int data_amount; // Data amount requested (if applicable)
    int initial_plafond; // Initial plafond of the user (if applicable)
    unsigned long long start_time; // Time when the request was made
} Request;

typedef struct Node {
    Request data;
    struct Node* next;
} Node;

typedef struct Queue {
    int num_elements;
    int max_elements;
    Node *front;
    Node *rear;
} Queue;

Queue* create_queue(int max_elements);
int is_empty(Queue* queue);
int is_full(Queue* queue);
void push(Queue* queue, Request data);
Request failed_request();
Request pop(Queue* queue);
Request front(Queue* queue);
Request rear(Queue* queue);


#endif
