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
#include <ctype.h>
#include <sys/msg.h>

#include "system_functions.h"
#include "global.h"
#include "queue.h"

// Writes a message to the log file
void write_to_log(char *message){
    // Wait if there is any process using the log file

    if(log_mutex_initialized){
        #ifdef DEBUG
        printf("<LOG>DEBUG# Waiting for log mutex\n");
        #endif
        pthread_mutex_lock(&auxiliary_shm->log_mutex);
        #ifdef DEBUG
        printf("<LOG>DEBUG# Locked log mutex\n");
        #endif
    }

    time_t current_time;
    // Struct to store time information
    struct tm* time_info;

    // Get current time
    time(&current_time);

    // Get time information
    time_info = localtime(&current_time);

    char time_str[100];

    // Write message to log file
    strftime(time_str, 100, "%Y-%m-%d %H:%M:%S", time_info);

    // Print message to console
    switch(current_process){
        case SYSMAN:
            printf("\033[1;33m");
            break;
        case ARM:
            printf("\033[1;34m");
            break;
        case AUTH_ENGINE:
            printf("\033[1;35m");
            break;
        case MONITOR_ENGINE:
            printf("\033[1;36m");
            break;
    }


    printf("[%s] %s\n", time_str, message);
    printf("\033[0m");

    // Print message to log file
    fprintf(log_file, "[%s] %s\n", time_str, message);

    // Unlock the log semaphore
    if(log_mutex_initialized){
        #ifdef DEBUG
        printf("<LOG>DEBUG# Unlocking log mutex\n");
        #endif
        pthread_mutex_unlock(&auxiliary_shm->log_mutex);
    }

    return;
}

// Prints the current state of the shared memory
void print_shared_memory(){
    #ifdef DEBUG
    printf("DEBUG# Printing current state of the shared memory...\n");
    #endif

    pthread_mutex_lock(&auxiliary_shm->log_mutex); // LOCK LOG MUTEX
    printf("\033[1;37m");

    int total_users = shared_memory->num_users;
    int already_printed = 0;
    char *up_r = "╔", *up_l = "╗", *down_r = "╣", *down_l = "╠";

    for(int i = 0; i < config->MOBILE_USERS; i++){
        if(shared_memory->users[i].isActive == 1){
            int initial = shared_memory->users[i].initial_plafond;
            int spent = shared_memory->users[i].spent_plafond;
            int remaining = initial - spent;

            // Only print the top border if it's the first user
            if(already_printed == 0){
                printf("\n%s═════════════════════════════════════════════════════════════════════════════%s", up_r, up_l);
            }
            printf("\n");          
           
            if(already_printed == total_users - 1){
                down_r = "╝";
                down_l = "╚";
            }
            else{
                down_r = "╣";
                down_l = "╠";
            }

            already_printed++;

            printf("║ User %-71d║\n", shared_memory->users[i].user_id);
            printf("║ \tInitial Plafond: %-53d║\n", initial);
            printf("║ \tSpent Plafond: %-55d║\n", spent);
            printf("║ \tRemaining Plafond: %-51d║\n", remaining);

            printf("║ \tProgress: [");
            int progress = (int)(((double)spent / initial) * 50);
            for(int j = 0; j < 50; j++){
                if(j < progress){
                    printf("#");
                } else {
                    printf("-");
                }
            }
            printf("]        ║\n");
            printf("%s═════════════════════════════════════════════════════════════════════════════%s", down_l, down_r);

            // The bottom border of the current user becomes the top border of the next user
            up_r = down_l;
            up_l = down_r;
            down_r = "╣";
            down_l = "╠";
        }
    }

    printf("\033[0m\n\n");
    pthread_mutex_unlock(&auxiliary_shm->log_mutex);
   // sem_post(log_semaphore); // Release stdout

}

// Sleeps for the specified amount of milliseconds
void sleep_milliseconds(int milliseconds){
    struct timespec ts;
    // Get time in seconds
    ts.tv_sec = milliseconds / 1000;
    // Get the remaining milliseconds and convert them to nanoseconds
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    // Sleep for the specified time
    nanosleep(&ts, NULL);
}

// Prints the queues progress
void print_progress(int current, int max){
    int barWidth = 70;

    printf("[");
    int pos = barWidth * current / max;
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) printf("=");
        else if (i == pos) printf(">");
        else printf(" ");
    }
    printf("] %d%%\n", (int)(current * 100.0 / max));
}

// Calls print_progress on both of the queues
void print_queues(int color){
    /*
        Color:
            1 - Red
            2 - Green
    */
    switch(color){
        case 1:
            printf("\033[1;31m");
            break;
        case 2:
            printf("\033[1;32m");
            break;
    }
   
    pthread_mutex_lock(&auxiliary_shm->log_mutex); // LOCK LOG MUTEX
    
    printf("\n-> Current state of the queues <-\n");


    printf("Video Queue:\n");
    print_progress(video_queue->num_elements, video_queue->max_elements);

    printf("Other Queue:\n");
    print_progress(other_queue->num_elements, other_queue->max_elements);

    printf("\n\033[0m");


    pthread_mutex_unlock(&auxiliary_shm->log_mutex); // UNLOCK LOG MUTEX
}

// Gets the current timestamp in milliseconds
unsigned long long get_time_millis(){
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (unsigned long long)(tv.tv_sec) * 1000 + (unsigned long long)(tv.tv_usec) / 1000;
}