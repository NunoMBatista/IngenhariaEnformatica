/*
  Authors: 
    Nuno Batista uc2022216127
    Miguel Castela uc2022212972
*/
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <semaphore.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/msg.h>
#include <sys/ipc.h>

#include "global.h"

#define BACKOFFICE_SEMAPHORE "backoffice_semaphore"
#define MAIN_STRING "\n\t    -> data_stats - Gets consumption statistics\n\t    -> reset - Resets the statistics\n\t    -> exit - Exits the backoffice user interface\n\n\n"
#define BACKOFFICE_TITLE "\n\n\t  ____             _       ____   __  __ _          \n\t |  _ \\           | |     / __ \\ / _|/ _(_)         \n\t | |_) | __ _  ___| | __ | |  | | |_| |_ _  ___ ___ \n\t |  _ < / _` |/ __| |/ / | |  | |  _|  _| |/ __/ _ \\\n\t | |_) | (_| | (__|   <  | |__| | | | | | | (_|  __/\n\t |____/ \\__,_|\\___|_|\\_\\  \\____/|_| |_| |_|\\___\\___|\n"
#define SHARED_MEMORY_TITLE "\t╔═╗┬ ┬┌─┐┬─┐┌─┐┌┬┐  ╔╦╗┌─┐┌┬┐┌─┐┬─┐┬ ┬\n\t╚═╗├─┤├─┤├┬┘├┤  ││  ║║║├┤ ││││ │├┬┘└┬┘\n\t╚═╝┴ ┴┴ ┴┴└─└─┘─┴┘  ╩ ╩└─┘┴ ┴└─┘┴└─ ┴ \n\n"

/*
    Execution instructions:
    ./backoffice_user
*/

void signal_handler(int signal);
void clean_up();
void interpret_command(char *command);
void send_message(char *message);
void *receiver();
void print_statistics(char *message);

int stop_receiver = 0; // Flag to stop the receiver thread

int fd_back_pipe;
int back_msq_id;

pthread_t receiver_thread;

int main(){
    // Check if the backoffice user can create the main lockfile
    int lockfile = open(MAIN_LOCKFILE, O_RDWR | O_CREAT, 0640);
    if (lockfile == -1){
        perror("open");
        return 1;
    }
    // If the lockfile was successfully locked, the system is not online
    if(lockf(lockfile, F_TLOCK, 0) == 0){ // The lock was successfully apllied
        printf("\033[31m!!! THE SYSTEM IS OFFLINE !!!\n\033[0m");
        unlink(MAIN_LOCKFILE); // Remove the lockfile
        return 1;
    }

    // Create a lockfile to prevent multiple backoffice users
    lockfile = open(BACKOFFICE_LOCKFILE, O_RDWR | O_CREAT, 0640); 
    if(lockfile == -1){
        perror("<ERROR> Could not create lockfile\n");
        return 1;
    }
    // Try to lock the file
    if(lockf(lockfile, F_TLOCK, 0) == -1){
        printf("!!! THERE CAN ONLY BE ONE BACKOFFICE USER !!!\n");
        return 1;
    }    

    // Clear the screen
    printf("\033[H\033[J");


    #ifdef DEBUG
    printf("DEBUG# Backoffice user - PROCESS ID: %d\n", getpid());
    printf("DEBUG# Redirecting SIGINT to signal handler\n");
    #endif
    signal(SIGINT, signal_handler);
    
    char message[100];
    sprintf(message, "BACKOFFICE USER STARTING - PROCESS ID: %d", getpid());
    printf("%s", message);

    #ifdef DEBUG
    printf("DEBUG# Creating receiver thread");
    #endif
    // Create a thread to receive messages from the system
    if(pthread_create(&receiver_thread, NULL, receiver, NULL) != 0){
        perror("<ERROR> Could not create receiver thread\n");
        return 1;
    }

    #ifdef DEBUG
    printf("DEBUG# Opening back pipe\n");
    #endif
    if((fd_back_pipe = open(BACK_PIPE, O_WRONLY)) < 0){
        perror("<ERROR> Could not open back pipe\n");
        return 1;
    }
    
    // Wait for a command 
    char command[100];
    while(1){

        printf("\033[1;32m");
        printf(BACKOFFICE_TITLE);

        printf(MAIN_STRING);
        printf("\n$ ");

        if(fgets(command, 100, stdin) == NULL){
            perror("<ERROR> Could not read command\n");
            return 1;
        }

        // Remove newline character
        if(command[strlen(command) - 1] == '\n'){
            command[strlen(command) - 1] = '\0';
        }
        
        if(stop_receiver){
            break;
        }

        // clear the screen
        printf("\033[H\033[J");
        printf("\033[0m");

        interpret_command(command);        
    }
}

void *receiver(){
    key_t queue_key = ftok(MESSAGE_QUEUE_KEY, 'a');
    if((back_msq_id = msgget(queue_key, 0777)) == -1){
        perror("<ERROR> Could not get message queue\n");
        return NULL;
    }

    #ifdef DEBUG
    printf("DEBUG# Receiver thread started\n");
    #endif
  
    char message_copy[PIPE_BUFFER_SIZE];
    QueueMessage qmsg;

    while(!stop_receiver){
        if(msgrcv(back_msq_id, &qmsg, sizeof(QueueMessage), 1, 0) == -1){
            if(errno == EIDRM){ // Message queue was removed
                signal(SIGINT, SIG_IGN); // Ignore SIGINT signal (CTRL+C
                printf("\033[H\033[J\033[31m!!! THE SYSTEM IS OFFLINE !!!\n[ENTER] to continue\033[0m");
                stop_receiver = 1;
                exit(0); // Exit the backoffice user interface
            }
            perror("<ERROR> Could not receive message\n");
            return NULL;
        }

        strcpy(message_copy, qmsg.text);

        char *token = strtok(message_copy, "#");

        if(strcmp(token, "SHM") == 0){
            print_statistics(qmsg.text);
        }

    }

    return NULL;
}


void interpret_command(char *command){
    char message[PIPE_BUFFER_SIZE] = "1#";

    if((strcmp(command, "data_stats") == 0) || strcmp(command, "reset") == 0){
        strcat(message, command);
        printf("\033[32mSending request... \t<%s>\033[0m", message);
        send_message(message);
        return;
    }

    if(strcmp(command, "exit") == 0){
        printf("Exiting backoffice user interface...\n");
        clean_up();
        exit(0);
    }
    else if(strcmp(command, "") == 0){
        return;
    }
    else{
        printf("\033[31mInvalid command\033[0m");
    }

}

void send_message(char *message){
    if(write(fd_back_pipe, message, strlen(message) + 1) < 0){
        perror("<ERROR> Could not write to back pipe\n");
        return;
    }
}
    
void signal_handler(int signal){
    if(signal == SIGINT){
        printf("\tExiting backoffice user interface\n");
        clean_up();
        exit(0);
    }
}

void clean_up(){
    signal(SIGINT, SIG_IGN); // Ignore SIGINT signal (CTRL+C)
    // We don't need a mutex to access the flag because writing to a int is atomic
    stop_receiver = 1;

    QueueMessage qmsg;
    qmsg.type = 1;
    strcpy(qmsg.text, "EXIT");

    // Send dummy message to unblock the receiver
    if(msgsnd(back_msq_id, &qmsg, sizeof(QueueMessage), IPC_NOWAIT) == -1){
        perror("<ERROR> Could not send message to message queue\n");
    }
    
    // Wait for the receiver thread to finish
    pthread_join(receiver_thread, NULL);
    
    // Close the back pipe
    close(fd_back_pipe);

    // Remove the lockfile
    unlink(BACKOFFICE_LOCKFILE);
}               

void print_statistics(char *message){
    int spent_video, spent_music, spent_social;
    int reqs_video, reqs_music, reqs_social;
    char *separator = "░░░▒▒▓█▓▒▒░░░";

    // clear the screen
    printf("\033[H\033[J");

    // printf("\033[1;36m+---------------------------------------------+\n");
    // printf("| RECEIVED CURRENT STATE OF THE SHARED MEMORY |\n");
    // printf("+---------------------------------------------+\n\n");

    printf("\033[1;36m");
    printf(SHARED_MEMORY_TITLE);

    // Token 1 is the shared memory type key
    char *token = strtok(message, "#");

    // Token 2 the amount of data spent on video
    token = strtok(NULL, "#");
    spent_video = atoi(token);

    // Token 3 is the amount of video requests
    token = strtok(NULL, "#");
    reqs_video = atoi(token);

    // Token 4 is the amount of data spent on music
    token = strtok(NULL, "#");
    spent_music = atoi(token);

    // Token 5 is the amount of music requests
    token = strtok(NULL, "#");
    reqs_music = atoi(token);

    // Token 6 is the amount data spent on social 
    token = strtok(NULL, "#");
    spent_social = atoi(token);

    // Token 7 is the amount of social requests
    token = strtok(NULL, "#");
    reqs_social = atoi(token);

    // Print everything
    printf("\t       \033[1;33m+---------------------+\n");
    printf("\t       |        Video:       |\n");
    printf("\t       +---------------------+\n");
    printf("\t       | Data spent: %7d |\n", spent_video);
    printf("\t       | Requests:   %7d |\n", reqs_video);
    printf("\t       +---------------------+\n");
     
    printf("\n\t            %s\n\n", separator);
   
    printf("\t       \033[1;33m+---------------------+\n");
    printf("\t       |        Music:       |\n");
    printf("\t       +---------------------+\n");
    printf("\t       | Data spent: %7d |\n", spent_music);
    printf("\t       | Requests:   %7d |\n", reqs_music);
    printf("\t       +---------------------+\n");
     
    printf("\n\t            %s\n\n", separator);
   
   
    printf("\t       \033[1;33m+---------------------+\n");
    printf("\t       |        Social:      |\n");
    printf("\t       +---------------------+\n");
    printf("\t       | Data spent: %7d |\n", spent_social);
    printf("\t       | Requests:   %7d |\n", reqs_social);
    printf("\t       +---------------------+\n\n");
    
    printf("\n\033[1;34m[ENTER] to continue\n\033[0m");   
}
