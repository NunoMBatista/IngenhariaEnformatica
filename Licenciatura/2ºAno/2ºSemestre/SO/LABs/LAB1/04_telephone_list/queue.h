#ifndef QUEUE_H
#define QUEUE_H

typedef struct queue_node{
    int num; 
    struct queue_node *next;
}queue_node;

typedef struct queue{
    queue_node *start;
    queue_node *end;
}queue;

void create(queue * q);
void insert(queue * q, int num);
int is_empty(queue * q);
int pop(queue * q);
void print_queue(queue * q);

#endif