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
#include <fcntl.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <pthread.h>

#include "message_struct.c"


#define BACKEND_PIPE "/tmp/backend_pipe"

#define BUF_SIZE 1024
#define MESSAGE_QUEUE 1234




int auth5g_request(char *req_type);

void *user_input();
void *messagequeue_response();


int backendpipe_fd;
int message_queue_id;


int main(int argc, char *argv[]) {
    if (argc!=1) {
        printf("!!!INCORRECT ARGUMENTS!!!\n-> %s\n", argv[0]);
    }

    if ((backendpipe_fd = open(BACKEND_PIPE, O_WRONLY)) < 0) {
        fprintf(stderr, "[ERROR]: Cannot open pipe for writing");
        exit(1);
    }

    if ( (message_queue_id = msgget(MESSAGE_QUEUE, 0666)) < 0 ) {
        fprintf(stderr, "[ERROR]: Cannot open message queue");
        exit(2);
    }

    pthread_t user_input_thread, backend_response_thread;
    pthread_create(&user_input_thread, NULL, user_input, NULL);
    pthread_create(&backend_response_thread, NULL, messagequeue_response, NULL);

    pthread_join(user_input_thread, NULL);
    pthread_join(backend_response_thread, NULL);

    return 0;
}


int auth5g_request(char *req_type) {
    /* Make request of type "req_type" */
    char buff_out[BUF_SIZE];            // Store messages to write to pipe

    sprintf(buff_out, "%d#%s", getpid(), req_type);
    printf("MAKE REQUEST: \"%s\"\n", buff_out);
    if (write(backendpipe_fd, &buff_out, sizeof(buff_out))==0 ) {
        fprintf(stderr, "[ERROR]: Cannot write to pipe");
        return 1;
    }

    return 0;
}


void *user_input() {
    /* Thread that handles the user input, and writes to pipe */
    char option[64];
    system("clear");
    while (1) {
        fprintf(stdout, "Backoffice\n");
        fprintf(stdout, "\t0: data_stats\n"
                        "\t1: reset\n"
                        "\t2: exit\n");
        /*
        fprintf(stdout, "\n0: data_stats - apresenta estatísticas referentes aos consumos dos dados nos vários serviços:\n"
        "total de dados reservados e número de pedidos de renovação de autorização;\n"
        "1: reset - limpa as estatísticas relacionadas calculadas até ao momento pelo sistema.\n"
        "2: exit - termina a execução do programa.\n");
        */
        fgets(option, 64, stdin);
        system("clear");
        if (option[1]!='\n') {
            fprintf(stderr, "[ERROR]: Invalid option\n");
        } else if (option[0]=='0') {
            if( auth5g_request("data_stats")!=0 ) {
                exit(1);;
            }
        } else if (option[0]=='1') {
            if ( auth5g_request("reset")!=0 ) {
                exit(1);
            }
        } else if (option[0]=='2') {
            fprintf(stdout, "Exiting...\n");
            exit(0);
        } else {
            fprintf(stderr, "[ERROR]: Invalid option\n");
        }
    }
    return NULL;
}

void *messagequeue_response() {
    /* Thread that reads the message queue */
    message msg_in;

    while (1) {
        if (msgrcv(message_queue_id, &msg_in, sizeof(msg_in), getpid(), 0) < 0) {
            fprintf(stderr, "[ERROR]: Cannot read from message queue\n");
            exit(2);
        }
        printf("RECEIVED: \"%s\"\n", msg_in.mtext);
    }
    return NULL;
}