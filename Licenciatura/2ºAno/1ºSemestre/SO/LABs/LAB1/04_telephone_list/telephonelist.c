#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

void store_queue(queue * q){
    FILE *fptr;
    fptr = fopen("numberlist.txt", "w");

    if(is_empty(q))
        return;

    queue_node * ptr;
    ptr = q->start;
    
    char str[10];
    while(ptr != NULL){
        sprintf(str, "%d\n", ptr->num);
        fprintf(fptr, str);
        ptr = ptr->next;
    }
}

queue* load_queue(){
    FILE *fptr;
    fptr = fopen("numberlist.txt", "r");

    queue *q = (queue*)malloc(sizeof(queue));
    create(q);

    char buffer[10];
    while(fgets(buffer, 10, fptr)){
        insert(q, atoi(buffer));    
    }
    return q;
}

int main (void){
    queue *q = (queue*)malloc(sizeof(queue));
    q = load_queue();

    while(1){
        int opt;
        printf("1 - Insert Number\n"
            "2 - Pop Number\n"
            "3 - Print List\n"
            "4 - Save\n"
            "5 - Quit\n");
            
        scanf("%d", &opt);
        system("clear");
        switch(opt){
            case 1:
                int add;
                printf("Input number: ");
                scanf("%d", &add);
                insert(q, add);
                break;
            case 2:
                printf("Popped %d\n", pop(q));
                break;
            case 3:
                print_queue(q);
                break;
            case 4:
                store_queue(q);
                break;
            case 5:
                return 0;
                break;
            default:
                printf("Not an option\n");
        }
    }
    
    return 0;
}