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
#include <string.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <pthread.h>
#include <semaphore.h>

#include "message_struct.c"



#define MOBILE_PIPE "/tmp/mobile_pipe"

#define BUF_SIZE 1024
#define MESSAGE_QUEUE 1234


typedef struct Settings {
    int init_plafond;
    int max_request;
    double video_interval, music_interval, social_interval;
    int request_size;
} Settings;

typedef struct Request {
    char *type;
    double interval;
} Request;



int validate_settings();

void *timed_request(void *req_p);
void *messagequeue_response();

int auth5g_register();
int auth5g_request(char *req_type);
int response_handler(char *response);



// Hold all the settings for this mobile user
struct Settings settings;
pthread_mutex_t mutex_requests = PTHREAD_MUTEX_INITIALIZER;
int requests_left;

int mobilepipe_fd;
int message_queue_id;



int main(int argc, char *argv[]) {

    if (argc!=7) {
        fprintf(stdout, "!!!INCORRECT ARGUMENTS!!!\n-> %s {plafond inicial}\n{número máximo de pedidos de autorização}\n{intervalo VIDEO} {intervalo MUSIC} {intervalo SOCIAL}\n{dados a reservar}\n", argv[0]);
        return 1;
    }

    settings.init_plafond = atoi(argv[1]);
    settings.max_request = atoi(argv[2]);
    requests_left = settings.max_request;

    settings.video_interval = (double)atoi(argv[3]);
    settings.music_interval = (double)atoi(argv[4]);
    settings.social_interval = (double)atoi(argv[5]);

    settings.request_size = atoi(argv[6]);


    fprintf(stdout, "Creating user:\n\tinitial plafond->%d\n\tmax requests->%d\n\tvideo interval->%f\n\tmusic interval->%f\n\tsocial interval->%f\n\trequest size->%d\n", settings.init_plafond, settings.max_request, settings.video_interval, settings.music_interval, settings.social_interval, settings.request_size);

    if ( validate_settings() != 0 ) {
        return 1;
    }

    if ((mobilepipe_fd = open(MOBILE_PIPE, O_WRONLY)) < 0) {
        fprintf(stderr, "[ERROR]: Cannot open pipe for writing\n");
        exit(1);
    }

    if ( (message_queue_id = msgget(MESSAGE_QUEUE, 0666)) < 0 ) {
        fprintf(stderr, "[ERROR]: Cannot open message queue\n");
        exit(2);
    }


    if (auth5g_register()!=0) {
        return -1;
    }

    pthread_t requesters[3], messagereciever;

    struct Request video_req = {"VIDEO", settings.video_interval};
    pthread_create(&requesters[0], NULL, timed_request, &video_req);
    struct Request music_req = {"MUSIC", settings.music_interval};
    pthread_create(&requesters[1], NULL, timed_request, &music_req);
    struct Request social_req = {"SOCIAL", settings.social_interval};
    pthread_create(&requesters[2], NULL, timed_request, &social_req);
    pthread_create(&messagereciever, NULL, messagequeue_response, NULL);

    pthread_join(requesters[0], NULL);
    pthread_join(requesters[1], NULL);
    pthread_join(requesters[2], NULL);
    pthread_cancel(messagereciever);        // kill messagereciever when all requesters are done

    // destroy the mutex
    pthread_mutex_destroy(&mutex_requests);

    return 0;
}



int validate_settings() {
    /* Returns 0 if "settings" is up to code;
     * Returns 1 if "settings" is wrongly configured, and prints to stderr what is wrong;
     */
    int flag = 0;
    if (settings.init_plafond <= 0) {
        fprintf(stderr, "[ERROR_ARGS]: {plafond inicial} must be >= 1\n");
        flag = 1;
    }
    if (settings.max_request <= 0) {
        fprintf(stderr, "[ERROR_ARGS]: {número máximo de pedidos de autorização} must be >= 1\n");
        flag = 1;
    }
    if (settings.video_interval <= 0) {
        fprintf(stderr, "[ERROR_ARGS]: {intervalo VIDEO} must be >= 1\n");
        flag = 1;
    }
    if (settings.music_interval <= 0) {
        fprintf(stderr, "[ERROR_ARGS]: {intervalo MUSIC} must be >= 1\n");
        flag = 1;
    }
    if (settings.social_interval <= 0) {
        fprintf(stderr, "[ERROR_ARGS]: {intervalo SOCIAL} must be >= 1\n");
        flag = 1;
    }
    if (settings.request_size <= 0) {
        fprintf(stderr, "[ERROR_ARGS]: {dados a reservar} must be >= 1\n");
        flag = 1;
    }
    return flag;
}


void *timed_request(void *req_p) {
    /* Make requests of type "req->type" with interval "req->interval" */
    struct Request *req = (struct Request *) req_p;
    while (1) {
        usleep(1000*req->interval);
        if (auth5g_request(req->type)!=0) {
            return NULL;
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
        //printf("RECEIVED: \"%s\"\n", msg_in.mtext);
        if (response_handler(msg_in.mtext)==-1) {
            exit(0);
        }
    }
    return NULL;
}


int auth5g_register() {
    /* Register on the 5g_auth system 
     *
     * Return:
     *      0 if successful
     *      -1 if writing failed
     * 
     * Register format: "{pid}#{plafond}"
     */
    char buff_out[BUF_SIZE];
    sprintf(buff_out, "%d#%d", getpid(), settings.init_plafond);
    fprintf(stdout, "[REGISTER]: \"%s\"\n", buff_out);
    if ( write(mobilepipe_fd, &buff_out, sizeof(buff_out))==0 ) {
        fprintf(stderr, "[ERROR]: Cannot write to \"%s\" pipe\n", MOBILE_PIPE);
        return -1;
    }
    return 0;
}

int auth5g_request(char *req_type) {
    /* Make request of type "req_type"
     *
     * Return:
     *      0 if successful
     *      1 if no requests left
     *      -1 if failed
     *
     * Request format: "{pid}#{req_type}#{request_size}"
     */
    char buff_out[BUF_SIZE];
    pthread_mutex_lock(&mutex_requests);
    if (requests_left==0) {
        fprintf(stdout, "no requests left\n");
        pthread_mutex_unlock(&mutex_requests);
        return 1;
    }
    sprintf(buff_out, "%d#%s#%d", getpid(), req_type, settings.request_size);
    fprintf(stdout, "[REQUEST #%d]: \"%s\"\n", requests_left, buff_out);
    if ( write(mobilepipe_fd, &buff_out, sizeof(buff_out))==0 ) {
        fprintf(stderr, "[ERROR]: Cannot write to \"%s\" pipe\n", MOBILE_PIPE);
        pthread_mutex_unlock(&mutex_requests);
        return -1;
    }
    requests_left--;
    pthread_mutex_unlock(&mutex_requests);
    return 0;
}

int response_handler(char *response) {
    /* Handle the response from the system */
    if (strcmp(response, "REJECTED")==0) {
        fprintf(stdout, "[FROM SYS] Mobile user rejected by system...\n");
        return -1;
    } else if (strcmp(response, "ACCEPT")==0) {
        fprintf(stdout, "[FROM SYS] Mobile user accepted by system!\n");
        return 0;
    } else if (strcmp(response, "DISCONNECT")==0) {
        fprintf(stdout, "[FROM SYS] Mobile user disconnected by system...\n");
        return -1;
    } else {
        fprintf(stderr, "[ERROR]: Unknown response \"%s\"\n", response);
        return 1;
    }
    return 1;
}