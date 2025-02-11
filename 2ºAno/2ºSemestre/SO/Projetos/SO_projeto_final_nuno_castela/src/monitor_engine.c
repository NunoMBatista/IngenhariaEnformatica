/*
  Authors: 
    Nuno Batista uc2022216127
    Miguel Castela uc2022212972
*/
#include <sys/shm.h>
#include <semaphore.h>
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
#include <fcntl.h>

#include "system_functions.h"
#include "global.h"
#include "queue.h"

pthread_t periodic_notifications_t;

volatile int monitor_exit = 0;

// Monitor Engine main function
// Will notify users once they have reached 80%, 90% and 100% of their plafond
void monitor_engine_process(){
    pthread_create(&periodic_notifications_t, NULL, periodic_notifications_thread, NULL);

    while(1){
        // Wait for an authorization request to notify
        pthread_mutex_lock(&auxiliary_shm->monitor_engine_mutex);
        pthread_cond_wait(&auxiliary_shm->monitor_engine_cond, &auxiliary_shm->monitor_engine_mutex);
       
        if(monitor_exit != 0){
            exit(0);
        }

        #ifdef DEBUG
        printf("<ME>DEBUG# Received signal to check users\n");
        printf("\033[33m<ME>DEBUG# Waiting for shared memory semaphore\033[0m\n");
        #endif

        // Read the whole shared memory and find possible notifications to send
        sem_wait(shared_memory_sem);

        #ifdef DEBUG
        printf("\033[31m<ME>DEBUG# Locked shared memory\n\033[0m");
        #endif

        
        for(int i = 0; i < config->MOBILE_USERS; i++){
            // Be careful: modifying current_user does not modify the shared memory
            MobileUserData current_user = shared_memory->users[i];

            
            if(current_user.isActive){
                int total_spent = current_user.spent_plafond;
                int max_plafond = current_user.initial_plafond; 


                double percentage_spent;

                if(max_plafond != 0){
                    percentage_spent = ((double)total_spent / (double)max_plafond) * 100;
                }
                else{
                    percentage_spent = 100;
                }              

                int percentage_notification;
                #ifdef DEBUG
                printf("<ME>DEBUG# User %d has spent %.2f\n", current_user.user_id, percentage_spent);
                #endif
                
                if(percentage_spent < 80){
                    break;
                }
                
                if(percentage_spent >= 100){
                    percentage_notification = 100;
                }
                else if(percentage_spent >= 90){
                    percentage_notification = 90;
                }
                else if(percentage_spent >= 80){
                    percentage_notification = 80;
                }

                // Notify user
                if(current_user.already_notified != percentage_notification){
                    #ifdef DEBUG
                    printf("<ME>DEBUG# Notifying user %d that he has spent %d%% of his plafond\n", current_user.user_id, percentage_notification);
                    #endif

                    char log_mess[PIPE_BUFFER_SIZE];;
                    sprintf(log_mess, "NOTIFYING USER %d THAT HE HAS SPENT %d%% OF HIS PLAFOND", current_user.user_id, percentage_notification);
                    write_to_log(log_mess);
                    
                    switch(percentage_notification){
                        case 100:
                            if(current_user.already_notified == 80){
                                notify_user(current_user.user_id, 90);
                            }
                            else if(current_user.already_notified == 0){
                                notify_user(current_user.user_id, 80);
                                notify_user(current_user.user_id, 90);
                            }
                            break;
                        case 90:
                            if(current_user.already_notified == 0){
                                notify_user(current_user.user_id, 80);
                            }
                            break;
                    }

                    notify_user(current_user.user_id, percentage_notification);
                    // Make sure the user is not notified again
                    shared_memory->users[i].already_notified = percentage_notification;
                }   
                
                if(percentage_spent >= 100){
                    // Deactivate user
                    if(deactivate_user(current_user.user_id, i) != -1){
                        write_to_log("Exit message successfully send to user");
                    }
                }
            }
        }
        

        #ifdef DEBUG
        printf("<ME>DEBUG# Finished checking users\n");
        printf("\033[32m<ME>DEBUG# Unlocking shared memory\n\033[0m");
        #endif


        sem_post(shared_memory_sem);

        pthread_mutex_unlock(&auxiliary_shm->monitor_engine_mutex);
    }

}

// Send notifications to the backoffice user in intervals of 30 seconds
void *periodic_notifications_thread(){
    while(1){
        sleep(30);

        // Check if the backoffice user is online
        int lockfile = open(BACKOFFICE_LOCKFILE, O_RDWR | O_CREAT, 0640);
        if (lockfile == -1){
            perror("open");
            continue;
        }
        // If the lockfile was successfully locked, the system is not online
        if(lockf(lockfile, F_TLOCK, 0) == 0){ // The lock was successfully apllied
            printf("\033[31m!!! THE BACKOFFICE USER IS NOT ONLINE, PERIODIC STATS NOT SENT !!!\n\033[0m");
            unlink(BACKOFFICE_LOCKFILE); // Remove the lockfile
            continue;
        }

        char stats[PIPE_BUFFER_SIZE];

        sem_wait(shared_memory_sem);
        sprintf(stats, "SHM#%d#%d#%d#%d#%d#%d", shared_memory->spent_video, shared_memory->reqs_video, shared_memory->spent_music, shared_memory->reqs_music, shared_memory->spent_social, shared_memory->reqs_social);
        sem_post(shared_memory_sem);

        QueueMessage qmsg;
        qmsg.type = 1;
        strcpy(qmsg.text, stats);

        if(msgsnd(message_queue_id, &qmsg, sizeof(QueueMessage), 0) == -1){
            write_to_log("Error sending message to monitor engine");
        }
        #ifdef DEBUG
        printf("<ME>DEBUG# Sent stats to monitor engine\n");
        #endif
        
    }
}

// Deactivates a user, called by the monitor engine
int deactivate_user(int user_id, int user_index){
    shared_memory->users[user_index].isActive = 0;
    shared_memory->num_users--;
    
    QueueMessage qmsg; 
    qmsg.type = user_id; 
    sprintf(qmsg.text, "DIE");
    
    if(msgsnd(message_queue_id, &qmsg, sizeof(QueueMessage), IPC_NOWAIT) == -1){
        write_to_log("Error sending exit message to user");
        return -1;
    }

    return 0;
}

// Notifies user about the amount of data spent
int notify_user(int user_id, int percentage){
    QueueMessage qmsg; 
    qmsg.type = user_id; 
    sprintf(qmsg.text, "%d", percentage);

    #ifdef DEBUG
    printf("<ME>DEBUG# Notifying user %d that he has spent %d%% of his plafond\n", user_id, percentage);
    #endif
    
    if(msgsnd(message_queue_id, &qmsg, sizeof(QueueMessage), IPC_NOWAIT) == -1){
        write_to_log("Error sending message to monitor engine");
        return -1;
    }

    return 0;
}