#include <stdlib.h>
#include <stdio.h>
#include "queue.h"

int is_empty(queue * q){
    if(q->start == NULL)
        return 1;
    return 0;
}

void create(queue * q){
    q->start = NULL;
    q->end = NULL;
}

void insert(queue * q, int num){
    queue_node * ptr; 
    ptr = (queue_node*)malloc(sizeof(queue_node));
    if(ptr == NULL)
        return;

    ptr->num = num;
    ptr->next = NULL;

    if(is_empty(q))
        q->start = ptr;
    else
        q->end->next = ptr;
    q->end = ptr;
}

int pop(queue * q){
    queue_node * ptr;
    int num;
    if(!is_empty(q)){
        ptr = q->start;
        num = ptr->num;
        q->start = q->start->next;
        if(is_empty(q))
            q->end = NULL;
        free(ptr);
        return num; 
    }
    return 0;
}

void print_queue(queue * q){
    queue_node * ptr; 
    ptr = q->start;

    int k = 0; 
    while(ptr != NULL){
        printf("%d: %d\n", ++k, ptr->num);
        ptr = ptr->next;
    }
}