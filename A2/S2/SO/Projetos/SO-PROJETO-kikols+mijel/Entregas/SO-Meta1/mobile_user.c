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

int main(int argc, char *argv[]) {
    char buff_out[BUF_SIZE];            // Store messages to write to pipe
    int init_plafond;
    int max_request;
    int video_interval, music_interval, social_interval;
    int request_size;

    if (argc!=7) {
        printf("!!!INCORRECT ARGUMENTS!!!\n-> %s {plafond inicial}\n{número máximo de pedidos de autorização}\n{intervalo VIDEO} {intervalo MUSIC} {intervalo SOCIAL}\n{dados a reservar}\n", argv[0]);
        return 1;
    }

    init_plafond = atoi(argv[1]);
    max_request = atoi(argv[2]);

    video_interval = atoi(argv[3]);
    music_interval = atoi(argv[4]);
    social_interval = atoi(argv[5]);

    request_size = atoi(argv[6]);

    printf("Creating user:\n\tinitial plafond->%d\n\tmax requests->%d\n\tvideo interval->%d\n\tmusic interval->%d\n\tsocial interval->%d\n\trequest size->%d\n", init_plafond, max_request, video_interval, music_interval, social_interval, request_size);

    sprintf(buff_out, "%d#%d", getpid(), init_plafond);
    printf("REGISTO: \"%s\"\n", buff_out);

    return 0;
}