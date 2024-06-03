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

int auth_engine_index;
volatile sig_atomic_t auth_engine_exit = 0;

// Authorization engine process
int auth_engine_process(int id){ 
    auth_engine_index = id;

    #ifdef DEBUG
    printf("<AE%d>DEBUG# Auth engine started with PID %d\n", id, getpid());
    #endif

    // Mark as available
    sem_wait(aux_shm_sem);
    auxiliary_shm->active_auth_engines[id] = 0;
    sem_post(aux_shm_sem);

    // "Notify" the sender thread that an auth engine is available
    sem_post(engines_sem);


    while(1){
        #ifdef DEBUG
        printf("<AE%d>DEBUG# Auth engine is ready to receive a request\n", id);
        #endif

        char log_mess[PIPE_BUFFER_SIZE];
        sprintf(log_mess, "AUTHORIZATION_ENGINE %d READY", id);
        write_to_log(log_mess);

        //char message[PIPE_BUFFER_SIZE];
        Request request;
        // Wait for message
        read(auth_engine_pipes[id][0], &request, sizeof(Request));

        sem_wait(shared_memory_sem); // Lock shared memory

        if((request.request_type == 'K') || (auth_engine_exit)){
            #ifdef DEBUG
            printf("<AE%d>DEBUG# Exiting...\n", id);
            #endif     

            sem_post(shared_memory_sem);       
            break;
        }

        #ifdef DEBUG
        printf("<AE%d>DEBUG# Auth engine is now processing a request:\n\tUSER: %d\n\tTYPE: %c\n\tDATA: %d\n\tINITPLAF: %d\n", id, request.user_id, request.request_type, request.data_amount, request.initial_plafond);
        #endif

        sprintf(log_mess, "AUTHORIZATION_ENGINE %d IS NOW PROCESSING REQUEST: USER %d, TYPE %c, DATA %d, INITPLAF %d", id, request.user_id, request.request_type, request.data_amount, request.initial_plafond);
        write_to_log(log_mess);

        #ifdef SLOWMOTION
        sleep_milliseconds(config->AUTH_PROC_TIME * SLOWMOTION / 10);
        #endif
        sleep_milliseconds(config->AUTH_PROC_TIME); // Simulate processing time

        char log_message[PIPE_BUFFER_SIZE + 500]; // Write to the log
        char log_message_type[PIPE_BUFFER_SIZE]; // Used to write to the log the type of request processed
        char log_process_feedback[PIPE_BUFFER_SIZE]; // Used to write to the log the result of the request processing
       
        int return_code; // Identifies the return codes from functions
        char response[PIPE_BUFFER_SIZE]; // Send back to the user through the message queue
        int response_applicable = 0; // 1 if there needs to be a response sent to the message queue
        char type = request.request_type; // Make the code more readable
 
        #ifdef DEBUG
        printf("\033[33m<AE%d>DEBUG# Waiting for shared memory semaphore\n\033[0m", id);
        #endif
        //sem_wait(shared_memory_sem); // Lock shared memory
        #ifdef DEBUG
        printf("\033[31m<AE%d>DEBUG# Locked shared memory\n\033[0m", id);
        #endif
        
        int user_index = get_user_index(request.user_id);

        // If the user is not asking to be added and does not exist
        if((request.request_type != 'I') && (user_index == -1)){
            // THE USER DOES NOT EXIST   
            //response_applicable = 1; 
            
            type = -1; // Skip if-else block
            //sprintf(response, "DIE");

            sprintf(log_message_type, "INVALID REQUEST");
            sprintf(log_process_feedback, "USER %d DOES NOT EXIST", request.user_id);
        }

        // It's a registration request
        if(type == 'I'){
            sprintf(log_message_type, "USER REGISTRATION REQUEST");
            // Try to register user
            
            // There's a need to tell the user if the request was accepted or rejected  
            response_applicable = 1; 

            return_code = add_mobile_user(request.user_id, request.initial_plafond);
            if(return_code == 0){
                sprintf(response, "ACCEPTED");
                //sprintf(log_message, "AUTHORIZATION_ENGINE %d: USER REGISTRATION REQUEST (ID = %d) PROCESSING COMPLETED -> USER ADDED WITH INITIAL PLAFFOND OF %d", id, request.user_id, request.initial_plafond);
                sprintf(log_process_feedback, "USER ADDED WITH INITIAL PLAFOND OF %d", request.initial_plafond);
            }
            else if(return_code == 1){
                sprintf(response, "REJECTED");
                //sprintf(log_message, "AUTHORIZATION_ENGINE %d: USER REGISTRATION REQUEST (ID = %d) PROCESSING COMPLETED -> SHARED MEMORY IS FULL => USER NOT ADDED", id, request.user_id);
                sprintf(log_process_feedback, "SHARED MEMORY IS FULL => USER NOT ADDED");
            }
            else if(return_code == 2){
                sprintf(response, "REJECTED");
                //sprintf(log_message, "AUTHORIZATION_ENGINE %d: USER REGISTRATION REQUEST (ID = %d) PROCESSING COMPLETED -> USER ALREADY EXISTS => USER NOT ADDED", id, request.user_id);
                sprintf(log_process_feedback, "USER ALREADY EXISTS => USER NOT ADDED");
            }
        }

        // It's a data request
        if((type == 'V') || (type == 'M') || (type == 'S')){
            int add_stats = request.data_amount; // The amount of data to add to the user stats
            
            response_applicable = 1;
            sprintf(response, "DATA#%d", request.data_amount);

            return_code = remove_from_user(user_index, add_stats);

            // If the user had enough data and still has some left
            if(return_code == 1){
                sprintf(log_process_feedback, "REMOVED %d FROM USER %d", request.data_amount, request.user_id);
            }
            
            // If the user had enough data but reached 0
            if(return_code == 2){
                sprintf(log_process_feedback, "USER %d HAS REACHED 0 PLAFOND AFTER REMOVING %d", request.user_id, request.data_amount);
            }
            
            // The user didn't have enough data
            if(return_code == -1){
                // Don't add anything to the total stats
                add_stats = 0; 
                sprintf(log_process_feedback, "USER %d DOES NOT HAVE ENOUGH PLAFOND TO GET %d, REMOVING USER", request.user_id, request.data_amount);
            }

            // Get respective log message type and add the stats to the total
            switch(type){
                case 'V':
                    sprintf(log_message_type, "VIDEO AUTHORIZATION REQUEST");
                    shared_memory->spent_video += add_stats;
                    shared_memory->reqs_video++;
                    break;
                case 'M':
                    sprintf(log_message_type, "MUSIC AUTHORIZATION REQUEST");
                    shared_memory->spent_music += add_stats;
                    shared_memory->reqs_music++;
                    break;
                case 'S':
                    sprintf(log_message_type, "SOCIAL AUTHORIZATION REQUEST");
                    shared_memory->spent_social += add_stats;
                    shared_memory->reqs_social++;
                    break;
            }

        }
        
        // If it's a backoffice request to get the shared memory stats
        if(type == 'D'){
            sprintf(log_message_type, "BACKOFFICE DATA_STATS REQUEST");

            response_applicable = 1;
            /*
                Message format:
                SHM#<spent_video>#<reqs_video>#<spent_music>#<reqs_music>#<spent_social>#<reqs_social>            
            */
            sprintf(response, "SHM#%d#%d#%d#%d#%d#%d", shared_memory->spent_video, shared_memory->reqs_video, shared_memory->spent_music, shared_memory->reqs_music, shared_memory->spent_social, shared_memory->reqs_social);
            request.user_id = 1; // The backoffice user id

            sprintf(log_process_feedback, "BACKOFFICE DATA_STATS REQUEST PROCESSED");
        }

        // If it's a backoffice request reset the shared memory stats
        if(type == 'R'){
            sprintf(log_message_type, "BACKOFFICE RESET REQUEST");

            shared_memory->spent_video = 0;
            shared_memory->spent_music = 0;
            shared_memory->spent_social = 0;
            shared_memory->reqs_video = 0;
            shared_memory->reqs_music = 0;
            shared_memory->reqs_social = 0;

            sprintf(log_process_feedback, "BACKOFFICE RESET REQUEST PROCESSED");            
        }

        #ifdef SHARED_MEMORY_DISPLAY
        print_shared_memory();
        #endif

        #ifdef DEBUG
        printf("\033[32m<AE%d>DEBUG# Unlocking shared memory\n\033[0m", id);
        #endif

        sem_post(shared_memory_sem); // Unlock shared memory 
    
        // NOTIFY MONITOR ENGINE
        pthread_mutex_lock(&auxiliary_shm->monitor_engine_mutex);
        pthread_cond_signal(&auxiliary_shm->monitor_engine_cond);
        pthread_mutex_unlock(&auxiliary_shm->monitor_engine_mutex);
        
        sprintf(log_message, "AUTHORIZATION_ENGINE %d: %s -> %s", id, log_message_type, log_process_feedback);
        write_to_log(log_message);


        if(response_applicable){
            #ifdef DEBUG
            printf("<AE%d>DEBUG# Sending response to user: %s\n", id, response);
            #endif
            QueueMessage response_message;
            // The type must be the user's id for it to be delivered to the correct user
            response_message.type = request.user_id; 
            strcpy(response_message.text, response);
            if(msgsnd(message_queue_id, &response_message, sizeof(response_message.text), 0) == -1){
                write_to_log("<ERROR SENDING RESPONSE MESSAGE>");
            }
            #ifdef DEBUG
            printf("<AE%d>DEBUG# Response sent to user %d: %s\n", id, request.user_id, response);
            #endif
        }

        // Mark the auth engine as available
        sem_wait(aux_shm_sem);
        auxiliary_shm->active_auth_engines[id] = 0;
        sem_post(aux_shm_sem);

        if(auth_engine_exit){
            break;
        }

        // "Notify" the sender thread that an auth engine is available
        sem_post(engines_sem);

        #ifdef DEBUG
        printf("<AE%d>DEBUG# Auth engine finished processing request:\n\tUSER: %d\n\tTYPE: %c\n\tDATA: %d\n\tINITPLAF: %d\nIt's available again\n", id, request.user_id, request.request_type, request.data_amount, request.initial_plafond);
        #endif
    }

    // Mark the auth engine as unavailable
    sem_wait(aux_shm_sem);
    auxiliary_shm->active_auth_engines[id] = 1;
    sem_post(aux_shm_sem);
    
    #ifdef DEBUG
    printf("<AE%d>DEBUG# Auth engine exiting, freeing pipes...\n", id);
    #endif
    close(auth_engine_pipes[id][0]);
    free(auth_engine_pipes[id]);

    // Write exit message to log
    char log_msg[PIPE_BUFFER_SIZE];
    sprintf(log_msg, "AUTH ENGINE %d EXITING", auth_engine_index);
    write_to_log(log_msg);

    exit(0);
}

// Adds a mobile user to the shared memory, called by the auth engines
int add_mobile_user(int user_id, int plafond){
    /*
        Returns:
            0 - User added successfully
            1 - Shared memory is full
            2 - User already exists
    */
    #ifdef DEBUG
    printf("DEBUG# Adding user %d to shared memory\n", user_id);
    #endif

    // Check if the shared memory is full
    if(shared_memory->num_users == config->MOBILE_USERS){
        return 1;
    }

    for(int i = 0; i <= config->MOBILE_USERS; i++){
        if(shared_memory->users[i].isActive == 0){
            // Check if there's a user with the same id already in the shared memory
            if((shared_memory->users[i].user_id == user_id) && (shared_memory->users[i].isActive == 1)){ 
                write_to_log("<ERROR ADDING USER TO SHARED MEMORY> User already exists");
                return 2;
            }
            shared_memory->num_users++;
            shared_memory->users[i].isActive = 1; // Set user as active
            shared_memory->users[i].user_id = user_id; // Set user id
            shared_memory->users[i].initial_plafond = plafond; // Set initial plafond
            shared_memory->users[i].spent_plafond = 0; // Set spent plafond
            shared_memory->users[i].already_notified = 0; // Set already notified to 0
            break;
        }
    }
    
    #ifdef DEBUG
    printf("DEBUG# User %d added to shared memory\n", user_id);
    #endif

    char message[100];
    sprintf(message, "USER %d ADDED TO SHARED MEMORY", user_id);

    write_to_log(message);

    return 0;
}

// Gets user index in shared memory, called by the auth engines
int get_user_index(int user_id){
    /*
        Returns:
            -1 - User not found
            i - User index
            config->MOBILE_USERS - backoffice user
    */
    
    if(user_id == 1){
        return config->MOBILE_USERS;
    }
    
    for(int i = 0; i < config->MOBILE_USERS; i++){
        // Check if the user is active and has the same id
        if((shared_memory->users[i].isActive == 1) && (shared_memory->users[i].user_id == user_id)){
            return i;
        }
    }
    return -1; // User not found
}

// Removes an ammount from the user's plafond, called by the auth engines
int remove_from_user(int user_index, int amount){
    /*
        Returns:
            1 - Success and remaining > 0
            2 - Sucess and remaining = 0
            -1 - User does not have enough plafond
    */
    if(shared_memory->users[user_index].isActive){
        shared_memory->users[user_index].spent_plafond += amount;
        int remaining = shared_memory->users[user_index].initial_plafond - shared_memory->users[user_index].spent_plafond;
        if(remaining == 0){
            return 2;
        }
        if(remaining < 0){             
            shared_memory->users[user_index].spent_plafond = shared_memory->users[user_index].initial_plafond; // Do not exceede the limit   
            return -1;
        }
        if(remaining > 0){
            return 1;
        }
    }
    return -2; // User not found
}
