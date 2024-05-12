#ifndef MESSAGE_STRUCT_C
#define MESSAGE_STRUCT_C

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>


#ifndef BUF_SIZE
#define BUF_SIZE 1024
#endif


typedef struct message {
    long mtype;
    char mtext[BUF_SIZE];
} message;


#endif