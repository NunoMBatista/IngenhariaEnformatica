/* 
 * Versão de 2ªMeta da disciplina Sistemas Operativos
 * 
 * TRABALHO REALIZADO POR:
 *      Francisco Amado Lapa Marques Silva - 2022213583
 *      Miguel Moital Rodrigues Cabral Martins - 2022213951
 */

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