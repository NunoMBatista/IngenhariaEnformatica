#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


typedef struct stack{
    char c;
    struct stack *next;
}type_stack;




int is_empty(type_stack **s);
void push(type_stack **s, char c);
char pop(type_stack **s); // -1 se pilha vazia
void delete(type_stack **s);

void string_to_stack(type_stack **s, char *str);
void string_operations(type_stack **s, int *ops, size_t size_ops);

int main(int argc, char **argv) {
    char str[] = "abcde";
    int ops[] = {0,1,0,1,0,2,2,0};
    size_t size_ops = sizeof(ops)/sizeof(int);
    type_stack *s = NULL;
    string_to_stack(&s, str);
    string_operations(&s, ops, size_ops);
    delete(&s);
    return 0;
}




/* MADE WITH COPILOT (pode estar mal) */
int is_empty(type_stack **s){
    return *s == NULL;
}
void push(type_stack **s, char c){
    type_stack *new = (type_stack *)malloc(sizeof(type_stack));
    new->c = c;
    new->next = *s;
    *s = new;
}
char pop(type_stack **s){
    if(is_empty(s)){
        return -1;
    }
    type_stack *aux = *s;
    *s = (*s)->next;
    char c = aux->c;
    free(aux);
    return c;
}
void delete(type_stack **s){
    while(!is_empty(s)){
        pop(s);
    }
}



/* FEITO PELO KIKO */
void string_to_stack(type_stack **s, char *str){
    for(int i = 0; str[i] != '\0'; i++){
        push(s, str[i]);
    }
}

void string_operations(type_stack **s, int *ops, size_t size_ops) {
    /* 0: print, 1: addicionar char, 2: pop*/
    for(int o=0; o<(int)size_ops; o++){
        switch(ops[o]){
            case 0:
                printf("Current string: ");
                // create temp stack
                type_stack *temp = NULL;
                // copy stack to temp and print
                while (!is_empty(s)) {
                    char c = pop(s);
                    push(&temp, c);
                    printf("%c", c);
                }
                // copy temp back to stack
                while (!is_empty(&temp)) {
                    push(s, pop(&temp));
                }
                printf("\n");
                break;
            case 1:
                printf("Added: ");
                char c;
                scanf(" %c", &c);       // n vou usar get_char pq é uma má função
                push(s, c);
                break;
            case 2:
                printf("Removed: %c\n", pop(s));
                break;
        }
    }
}