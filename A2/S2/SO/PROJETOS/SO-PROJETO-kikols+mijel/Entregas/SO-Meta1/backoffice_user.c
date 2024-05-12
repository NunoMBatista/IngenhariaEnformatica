/* 
 * Versão de 1ªMeta da disciplina Sistemas Operativos
 * 
 * TRABALHO REALIZADO POR:
 *      Francisco Amado Lapa Marques Silva - 2022213583
 *      Miguel Moital Rodrigues Cabral Martins - 2022213951
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>


#define BUF_SIZE 1024

int main() {
    char buff_out[BUF_SIZE];            // Store messages to write to pipe
    sprintf(buff_out, "%d#%s", getpid(), "data_stats");
    printf("MAKE REQUEST: \"%s\"\n", buff_out);
    return 1;
}