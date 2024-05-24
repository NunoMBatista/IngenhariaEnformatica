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
#include <sys/ioctl.h>
#include <sys/msg.h>

#include "system_functions.h"
#include "global.h"
#include "queue.h"

// Cleans up the system, called by the signal handler
void clean_up(){
    signal(SIGINT, SIG_IGN); // Ignore SIGINT while cleaning up
    
    // Notify ARM to exit
    #ifdef DEBUG
    printf("<SYS MAN>DEBUG# Notifying ARM to exit\n");
    #endif
    kill(arm_pid, SIGTERM);

    write_to_log("Waiting for ARM process to finish");
    int status;
    // Wait for ARM to exit
    waitpid(arm_pid, &status, 0);


    // Notify monitor engine to exit
    #ifdef DEBUG
    printf("<SYS MAN>DEBUG# Notifying monitor engine to exit\n");
    #endif
    kill(monitor_pid, SIGTERM);

    write_to_log("Waiting for monitor engine to finish");
    //waitpid(monitor_pid, &status, 0);

    write_to_log("5G_AUTH_PLATFORM SIMULATOR CLOSING");

    // Kill extra auth engine if it's active   
    if(extra_auth_engine){
        kill(extra_auth_pid, SIGKILL);
    }

    // Destroy mutexes and condition variables
    pthread_mutex_destroy(&queues_mutex);
    pthread_cond_destroy(&sender_cond);
    pthread_mutex_destroy(&auxiliary_shm->monitor_engine_mutex);    
    pthread_cond_destroy(&auxiliary_shm->monitor_engine_cond);
    pthread_mutex_destroy(&auxiliary_shm->log_mutex);

    // Destroy mutex attributes structures
    pthread_mutexattr_destroy(&shared_mutex);
    pthread_condattr_destroy(&shared_cond);
    pthread_mutexattr_destroy(&log_mutex_attr);

    #ifdef DEBUG
    printf("<SYS MAN>DEBUG# Detatching and deleting the main shared memory\n");
    #endif
    // Deleted nested shared memory
    if(shared_memory->users != NULL){
        if(shmdt(shared_memory->users) == -1){
            write_to_log("<ERROR DETATCHING USERS SHARED MEMORY>");
        }
        if(shmctl(shm_id_users, IPC_RMID, NULL) == -1){
            write_to_log("<ERROR DELETING USERS SHARED MEMORY>");
        }
    }
    if(shared_memory != NULL){    
        // Detach and delete shared memory
        if(shmdt(shared_memory) == -1){
            write_to_log("<ERROR DETATCHING SHARED MEMORY>");
        }
        if(shmctl(shm_id, IPC_RMID, NULL) == -1){
            write_to_log("<ERROR DELETING SHARED MEMORY>");
        }
    }

    #ifdef DEBUG
    printf("<SYS MAN>DEBUG# Detatching and deleting the auxiliary shared memory\n");
    #endif
    if(auxiliary_shm->active_auth_engines != NULL){
        if(shmdt(auxiliary_shm->active_auth_engines) == -1){
            write_to_log("<ERROR DETATCHING ENGINES SHARED MEMORY>");
        }
        if(shmctl(engines_shm_id, IPC_RMID, NULL) == -1){
            write_to_log("<ERROR DELETING ENGINES SHARED MEMORY>");
        }
    }
    if(auxiliary_shm != NULL){
        // Detach and delete auxiliary shared memory
        if(shmdt(auxiliary_shm) == -1){
            write_to_log("<ERROR DETATCHING AUXILIARY SHARED MEMORY>");
        }
        if(shmctl(aux_shm_id, IPC_RMID, NULL) == -1){
            write_to_log("<ERROR DELETING AUXILIARY SHARED MEMORY>");
        }
    }

    #ifdef DEBUG
    printf("<SYS MAN>DEBUG# Destroying message queue\n");
    #endif
    // Destroy message queue
    if(msgctl(message_queue_id, IPC_RMID, NULL) == -1){
        write_to_log("<ERROR DESTROYING MESSAGE QUEUE>");
    }
    
    #ifdef DEBUG
    printf("<SYS MAN>DEBUG# Removing message queue key file\n");
    #endif
    // Remove message queue key file
    if(remove(MESSAGE_QUEUE_KEY) == -1){
        write_to_log("<ERROR REMOVING MESSAGE QUEUE KEY FILE>");
    }

    // Free config memory
    if(config != NULL){
        free(config);
    }

    #ifdef DEBUG
    printf("<SYS MAN>DEBUG# Closing and unlinking pipes\n");
    #endif
    // Close and unlink pipes
    close(fd_user_pipe);
    close(fd_back_pipe);
    unlink(USER_PIPE);
    unlink(BACK_PIPE);
    
    #ifdef DEBUG
    printf("<SYS MAN>DEBUG# Closing and unlinking semaphores\n");
    #endif
    // Close and unlink semaphores
    sem_close(log_semaphore);
    sem_unlink(LOG_SEMAPHORE);
    sem_close(shared_memory_sem);
    sem_unlink(SHARED_MEMORY_SEMAPHORE);
    sem_close(aux_shm_sem);
    sem_unlink(AUXILIARY_SHM_SEMAPHORE);
    sem_close(engines_sem);
    sem_unlink(ENGINES_SEMAPHORE);

    // Unlink lockfiles
    unlink(MAIN_LOCKFILE);
    unlink(BACKOFFICE_LOCKFILE);

    // Close log file
    fclose(log_file);
}

// Cleans up the ARM process and it's structures
void clean_up_arm(){
    // Notify ARM threads to exit    
    notify_arm_threads();

    #ifdef DEBUG
    printf("<ARM>DEBUG# Waiting for ARM threads to finish\n");
    #endif
    pthread_join(receiver_t, NULL);

    #ifdef DEBUG
    printf("<ARM>DEBUG# Receiver thread finished\n");
    #endif

    pthread_join(sender_t, NULL);

    #ifdef DEBUG
    printf("<ARM>DEBUG# Sender thread finished\n");
    #endif

    #ifdef DEBUG
    printf("<ARM>DEBUG# ARM threads finished\n");
    #endif

    // Write to log the unfufilled requests
    Request req;
    char unfufilled_request[PIPE_BUFFER_SIZE];    
    while(!is_empty(video_queue)){
        req = pop(video_queue);
        sprintf(unfufilled_request, "Unfulfilled request: %d#%c#%d#%d", req.user_id, req.request_type, req.data_amount, req.initial_plafond);
        write_to_log(unfufilled_request);
    }
    while(!is_empty(other_queue)){
        req = pop(other_queue);
        sprintf(unfufilled_request, "Unfulfilled request: %d#%c#%d#%d", req.user_id, req.request_type, req.data_amount, req.initial_plafond);
        write_to_log(unfufilled_request);
    }

    // extra_auth_engine = 0;
    // Let the auth engines know they should exit
    for(int i = 0; i < config->AUTH_SERVERS + 1; i++){
        if(i == config->AUTH_SERVERS){ // The last auth engine is the extra one
            if(!extra_auth_engine){ // Only kill it if it's active
                continue;
            }
            #ifdef DEBUG
            printf("<ARM>DEBUG# Killing extra auth engine\n");
            #endif
            extra_auth_engine = 0;
            kill(extra_auth_pid, SIGTERM);
        }
        else{
            #ifdef DEBUG
            printf("<ARM>DEBUG# Killing auth engine %d\n", i);
            #endif
            kill(auth_engine_pids[i], SIGTERM);
        }

        // Write dummy request in case the auth engine is waiting for a message in the pipe
        Request dummy;
        dummy.request_type = 'K';
        write(auth_engine_pipes[i][1], &dummy, sizeof(Request));
    }


    // Wait for the auth engines to leave
    #ifdef DEBUG
    printf("<ARM>DEBUG# Waiting for auth engines to finish\n");
    #endif
    while(wait(NULL) > 0);
    
    // Close and free every pipe
    #ifdef DEBUG
    printf("<ARM>DEBUG# Closing and freeing pipes\n");
    #endif       
    for(int i = 0; i < config->AUTH_SERVERS + 1; i++){
        close(auth_engine_pipes[i][1]);
        free(auth_engine_pipes[i]);
    }
    free(auth_engine_pipes);
 
    #ifdef DEBUG
    printf("<ARM>DEBUG# Freeing auth engine pids\n");
    #endif
    free(auth_engine_pids);

    #ifdef DEBUG
    printf("<ARM>DEBUG# Freeing queues\n");
    #endif
    free(video_queue);
    free(other_queue);

    #ifdef DEBUG
    printf("<ARM>DEBUG# ARM process exiting\n");
    #endif
}

// Ask ARM threads to exit
void notify_arm_threads(){
    #ifdef DEBUB
    printf("<ARM>DEBUG# Notifying ARM threads to exit\n");
    #endif
    arm_threads_exit = 1;

    #ifdef DEBUG
    printf("<ARM>DEBUG# Sending exit message to receiver thread\n");
    #endif
  
  
    // Send message to receiver thread in case it's waiting to read from a pipe
    char exit_message[PIPE_BUFFER_SIZE] = "EXIT";
    int bytes_available;
    if (ioctl(fd_user_pipe, FIONREAD, &bytes_available) == -1) { 
        write_to_log("<ERROR CHECKING PIPE>");
    } 
    else if (bytes_available == 0) {  // Only send the message if there are no bytes available in the pipe
        #ifdef DEBUG
        printf("<ARM>DEBUG# No bytes available in pipe, sending dummy message\n");
        #endif

        if(write(fd_user_pipe, exit_message, PIPE_BUFFER_SIZE) == -1){
            write_to_log("<ERROR SENDING EXIT MESSAGE TO RECEIVER THREAD>");
        }
    
        #ifdef DEBUG
        printf("<ARM>DEBUG# Dummy message sent to receiver thread\n");
        #endif
    } 


}

// Signal handler for SIGINT
void signal_handler(int signal){
    if(signal == SIGINT){
        if(current_process == SYSMAN){
            write_to_log("SIGNAL SIGINT RECEIVED");
            clean_up();
        }
        exit(0);
    }
    if(signal == SIGTERM){ 
        //if(getpid() == monitor_pid){
        if(current_process == MONITOR_ENGINE){
            #ifdef DEBUG
            printf("<ME>DEBUG# Received SIGTERM\n");
            #endif

            // CANCEL THREAD AND WAIT FOR IT TO JOIN (SLEEP IS A CANCELATION POINT)
            pthread_cancel(periodic_notifications_t);
            pthread_join(periodic_notifications_t, NULL);

            // Monitor engine exit flag
            monitor_exit = 1;

            // NOTIFY MONITOR ENGINE
            pthread_mutex_lock(&auxiliary_shm->monitor_engine_mutex);
            pthread_cond_signal(&auxiliary_shm->monitor_engine_cond);
            pthread_mutex_unlock(&auxiliary_shm->monitor_engine_mutex);
        }
        if(current_process == ARM){
            #ifdef DEBUG
            printf("<ARM>DEBUG# Received SIGTERM\n");
            #endif
            clean_up_arm();
            exit(0);
        }
        if(current_process == AUTH_ENGINE){
            #ifdef DEBUG
            printf("<AE>DEBUG# Received SIGTERM\n");
            #endif
            auth_engine_exit = 1;
        }
    }
}