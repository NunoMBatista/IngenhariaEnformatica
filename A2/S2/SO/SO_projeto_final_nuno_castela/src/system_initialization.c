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

// Initializes the system, called by the main function
int initialize_system(char* config_file){
    #ifdef DEBUG
    printf("<SYS MAN>DEBUG# Initializing system...\n");
    #endif

    #ifdef DEBUG
    printf("<SYS MAN>DEBUG# Setting up signal handler...\n");
    #endif
    // Signal handling
    signal(SIGINT, signal_handler);
    
    // The semaphores need to be set up before any other feature because they are used to write to the log file
    #ifdef DEBUG
    printf("<SYS MAN>DEBUG# Creating semaphores...\n");
    #endif
    write_to_log("CREATING SEMAPHORES");
    if(create_semaphores() != 0){
        return 1;
    }

    write_to_log("5G_AUTH_PLATFORM SIMULATOR STARTING");
    write_to_log("PROCESS SYSTEM_MANAGER CREATED");
    
    #ifdef DEBUG
    printf("<SYS MAN>DEBUG# Setting up config struct\n");
    #endif    
    // Allocate memory for the config struct
    config = (Config*) malloc(sizeof(Config));
    // Read the config file
    write_to_log("READING CONFIG FILE");
    if(read_config_file(config_file) != 0){
        return 1;
    }

    #ifdef DEBUG
    printf("<SYS MAN>DEBUG# Creating message queue...\n");
    #endif
    write_to_log("CREATING MESSAGE QUEUE");
    if(create_message_queue() != 0){
        return 1;
    }

    // The auxiliary semaphores must be created after reading the config file
    // Because the engines semaphore depends on the number of auth servers
    #ifdef DEBUG
    printf("Creating auxiliary semaphores...\n");
    #endif
    write_to_log("CREATING AUXILIARY SEMAPHORES");
    if(create_aux_semaphores() != 0){
        return 1;
    }

    #ifdef DEBUG
    printf("<SYS MAN>DEBUG# Creating shared memory...\n");
    #endif
    write_to_log("CREATING SHARED MEMORY");
    if(create_shared_memory() != 0){
        return 1;
    }
    log_mutex_initialized = 1;

    
    #ifdef DEBUG
    printf("<SYS MAN>DEBUG# Creating auth manager...\n");
    #endif
    write_to_log("CREATING AUTH MANAGER");
    if(create_auth_manager() != 0){
        return 1;
    }

    #ifdef DEBUG
    printf("<SYS MAN>DEBUG# Creating monitor engine...\n");
    #endif
    write_to_log("CREATING MONITOR ENGINE");
    if(create_monitor_engine() != 0){
        return 1;
    }


    #ifdef DEBUG
    printf("<SYS MAN>DEBUG# System initialized successfully\n");
    #endif
    write_to_log("SYSTEM INITIALIZED SUCCESSFULLY");
    return 0;
}

// Reads the configuration file and store the values in the global config struct
int read_config_file(char *filename){
    // Return 0 if the config file was read successfully and has valid values
    // Return 1 otherwise

    FILE* config_file = fopen(filename, "r");
    if(config_file == NULL){
        write_to_log("<ERROR OPENING CONFIG FILE>");
        return 1;
    }

    #ifdef DEBUG
    printf("Reading config file...\n");
    #endif

    // There should be 6 positive integers in the config file separated by \n
    int config_values = fscanf(config_file, "%d\n%d\n%d\n%d\n%d\n%d", &config->MOBILE_USERS, &config->QUEUE_POS, &config->AUTH_SERVERS, &config->AUTH_PROC_TIME, &config->MAX_VIDEO_WAIT, &config->MAX_OTHERS_WAIT);

    if(config_values != 6){
        write_to_log("<INCORRECT NUMBER OF CONFIG VALUES> There should be 6 configuration values");
        fclose(config_file);
        return 1;
    }

    // Check if there are more than 6 values
    int extra_values = 0;
    // Read until EOF
    while(fgetc(config_file) != EOF){
        extra_values++;
    }

    if(extra_values > 0){
        write_to_log("<EXTRA CONFIG VALUES> There are more than 6 configuration values in the config file, only the first 6 will be considered");
    }

    #ifdef DEBUG
    printf("Config values read successfully, checking validity...\n");
    #endif
    
    // Check if all values are positive integers
    if(config->MOBILE_USERS <= 0 || config->QUEUE_POS <= 0 || config->AUTH_SERVERS <= 0 || config->AUTH_PROC_TIME <= 0 || config->MAX_VIDEO_WAIT <= 0 || config->MAX_OTHERS_WAIT <= 0){
        write_to_log("<INVALID CONFIG VALUES> Every configuration value should be a positive integer");
        fclose(config_file);
        return 1;
    }

    #ifdef DEBUG
    printf("DEBUG#Config values are valid:\n");
    printf("\tMOBILE_USERS: %d\n\tQUEUE_POS: %d\n\tAUTH_SERVERS: %d\n\tAUTH_PROC_TIME: %d\n\tMAX_VIDEO_WAIT: %d\n\tMAX_OTHERS_WAIT: %d\n",
           config->MOBILE_USERS, config->QUEUE_POS, config->AUTH_SERVERS, config->AUTH_PROC_TIME, config->MAX_VIDEO_WAIT, config->MAX_OTHERS_WAIT);
    #endif

    write_to_log("CONFIG FILE READ SUCCESSFULLY");

    return 0;
}