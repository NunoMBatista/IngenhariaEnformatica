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

// Creates the monitor engine process
int create_monitor_engine(){   
 
    // Create a new process
    #ifdef DEBUG
    printf("<ME>DEBUG# Creating monitor engine...\n");
    #endif
    monitor_pid = fork();

    if(monitor_pid == -1){
        write_to_log("<ERROR CREATING MONITOR ENGINE>");
        return 1;
    }

    if(monitor_pid == 0){
        // Set process name
        memset(process_name, 0, process_name_size);
        strcpy(process_name, "5G_MONITOR_ENGINE");

        monitor_pid = getpid();
        current_process = MONITOR_ENGINE;

        signal(SIGTERM, signal_handler); // Only exit when receiving SIGTERM (called by system manager)
        signal(SIGINT, SIG_IGN); // Ignore SIGINT

        // Child process
        char mes[PIPE_BUFFER_SIZE];
        sprintf(mes, "<ME> MONITOR ENGINE STARTED WITH PID %d", getpid());
        write_to_log(mes);

        monitor_engine_process();
        
        exit(0);
    }

    // Parent process
    return 0;
}

// Creates the auxiliary shared memory semaphore and the engines semaphore
int create_aux_semaphores(){
    sem_close(aux_shm_sem);
    sem_unlink(AUXILIARY_SHM_SEMAPHORE);
    sem_close(engines_sem);
    sem_unlink(ENGINES_SEMAPHORE);

    #ifdef DEBUG
    printf("DEBUG# Creating auxiliary shared memory semaphore...\n");
    #endif

    // Create auxiliary shared memory semaphore
    aux_shm_sem = sem_open(AUXILIARY_SHM_SEMAPHORE, O_CREAT | O_EXCL, 0777, 1);
    if(aux_shm_sem == SEM_FAILED){
        write_to_log("<ERROR CREATING AUXILIARY SHM SEMAPHORE>");
        return 1;
    }

    #ifdef DEBUG
    printf("DEBUG# Auxiliary shared memory semaphore created successfully\n");
    printf("DEBUG# Creating engines semaphore...\n");
    #endif

    // Create engines semaphore
    engines_sem = sem_open(ENGINES_SEMAPHORE, O_CREAT | O_EXCL, 0777, 0); // Start as locked, will be unlocked when the auth engines are created
    if(engines_sem == SEM_FAILED){
        write_to_log("<ERROR CREATING ENGINES SEMAPHORE>");
        return 1;
    }

    #ifdef DEBUG
    printf("DEBUG# Engines semaphore created successfully\n");
    #endif
    return 0; 
}

// Creates the shared memory and auxiliary shared memory
int create_shared_memory(){   
    // Initialize shared memory struct
    shm_id = shmget(IPC_PRIVATE, sizeof(SharedMemory), IPC_CREAT | 0777);
    if(shm_id == -1){
        write_to_log("<ERROR CREATING SHARED MEMORY>");
        return 1;
    }
    shared_memory = (SharedMemory*) shmat(shm_id, NULL, 0);
    if(shared_memory == (void*) -1){
        write_to_log("<ERROR ATTACHING SHARED MEMORY>");
        return 1;
    }
    
    // Allocate memory for the users
    shm_id_users = shmget(IPC_PRIVATE, sizeof(MobileUserData) * config->MOBILE_USERS, IPC_CREAT | 0777);
    if(shm_id_users == -1){
        write_to_log("<ERROR CREATING SHARED MEMORY FOR USERS>");
        return 1;
    }
    shared_memory->users = (MobileUserData*) shmat(shm_id_users, NULL, 0);
    if(shared_memory->users == (void*) -1){
        write_to_log("<ERROR ATTACHING SHARED MEMORY FOR USERS>");
        return 1;
    }
    // Initialize the shared memory values
    shared_memory->num_users = 0;
    shared_memory->spent_video = 0;
    shared_memory->spent_social = 0;
    shared_memory->spent_music = 0;
    shared_memory->reqs_video = 0;
    shared_memory->reqs_social = 0;
    shared_memory->reqs_music = 0;
    
    for(int i = 0; i < config->MOBILE_USERS; i++){
        shared_memory->users[i].isActive = 0;
    }


    // Initialize auxiliary shared memory
    aux_shm_id = shmget(IPC_PRIVATE, sizeof(AuxiliaryShm), IPC_CREAT | 0777);
    if(aux_shm_id == -1){
        write_to_log("<ERROR CREATING AUXILIARY SHARED MEMORY>");
        return 1;
    }
    auxiliary_shm = (AuxiliaryShm*) shmat(aux_shm_id, NULL, 0);
    if(auxiliary_shm == (void*) -1){
        write_to_log("<ERROR ATTACHING AUXILIARY SHARED MEMORY>");
        return 1;
    }

    // Allocate memory for engines
    engines_shm_id = shmget(IPC_PRIVATE, sizeof(int) * (config->AUTH_SERVERS + 1), IPC_CREAT | 0777); // +1 for the extra engine
    if(engines_shm_id == -1){
        write_to_log("<ERROR CREATING AUXILIARY SHARED MEMORY FOR ENGINES>");
        return 1;
    }
    auxiliary_shm->active_auth_engines = (int*) shmat(engines_shm_id, NULL, 0);
    if(auxiliary_shm->active_auth_engines == (void*) -1){
        write_to_log("<ERROR ATTACHING AUXILIARY SHARED MEMORY FOR ENGINES>");
        return 1;
    }
    for(int i = 0; i < config->AUTH_SERVERS + 1; i++){
        // Set all auth engines as unavailable
        auxiliary_shm->active_auth_engines[i] = 1;
    }
    
    // Initialize shared mutex and condition variables
    // pthread_mutexattr_t shared_mutex;
    pthread_mutexattr_init(&shared_mutex); // Initialize mutex attributes
    pthread_mutexattr_setpshared(&shared_mutex, PTHREAD_PROCESS_SHARED); // Share mutex between processes
    if(pthread_mutex_init(&auxiliary_shm->monitor_engine_mutex, &shared_mutex) != 0){
        write_to_log("<ERROR INITIALIZING MONITOR ENGINE MUTEX>");
        return 1;
    }

    // pthread_condattr_t shared_cond;
    pthread_condattr_init(&shared_cond); // Initialize condition variable attributes
    pthread_condattr_setpshared(&shared_cond, PTHREAD_PROCESS_SHARED); // Share condition variable between processes
    if(pthread_cond_init(&auxiliary_shm->monitor_engine_cond, &shared_cond) != 0){
        write_to_log("<ERROR INITIALIZING MONITOR ENGINE CONDITION VARIABLE>");
        return 1;
    }

    // pthread_mutexattr_t log_mutex_attr;
    pthread_mutexattr_init(&log_mutex_attr);
    pthread_mutexattr_setpshared(&log_mutex_attr, PTHREAD_PROCESS_SHARED);
    if(pthread_mutex_init(&auxiliary_shm->log_mutex, &log_mutex_attr) != 0){
        write_to_log("<ERROR INITIALIZING LOG MUTEX>");
        return 1;
    }

    #ifdef DEBUG
    printf("DEBUG# Shared memory created successfully\n");
    #endif

    return 0;
}

// Creates the video and other queues
int create_fifo_queues(){
    #ifdef DEBUG
    printf("<ARM>DEBUG# Creating video queue...\n");
    #endif
    video_queue = create_queue(config->QUEUE_POS);
    
    #ifdef DEBUG
    printf("<ARM>DEBUG# Creating other queue...\n");
    #endif
    other_queue = create_queue(config->QUEUE_POS);
    
    return 0; 
}

// Creates the log semaphore and the shared memory semaphore
int create_semaphores(){
    // Clean up sempahores if they already exist
    sem_close(log_semaphore);
    sem_unlink(LOG_SEMAPHORE);
    sem_close(shared_memory_sem);
    sem_unlink(SHARED_MEMORY_SEMAPHORE);

    #ifdef DEBUG
    printf("DEBUG# Creating log semaphore...\n");
    #endif
    // Create log semaphore
    log_semaphore = sem_open(LOG_SEMAPHORE, O_CREAT | O_EXCL, 0777, 1);
    if(log_semaphore == SEM_FAILED){
        write_to_log("<ERROR CREATING LOG SEMAPHORE>");
        return 1;
    }

    #ifdef DEBUG
    printf("DEBUG# Log semaphore created successfully\n");
    printf("DEBUG# Creating shared memory semaphore...\n");
    #endif

    // Create shared memory semaphore
    shared_memory_sem = sem_open(SHARED_MEMORY_SEMAPHORE, O_CREAT | O_EXCL, 0777, 1);
    if(shared_memory_sem == SEM_FAILED){
        write_to_log("<ERROR CREATING SHARED MEMORY SEMAPHORE>");
        return 1;
    }

    #ifdef DEBUG
    printf("DEBUG# Shared memory semaphore created successfully\n");
    #endif

    return 0;
}

// Creates the ARM process, which will create the auth engines and the receiver/sender threads
int create_auth_manager(){
    arm_pid = fork();

    if(arm_pid == -1){
        write_to_log("<ERROR CREATING AUTH MANAGER>");
        return 1;
    }

    if(arm_pid == 0){
        // Set process name
        memset(process_name, 0, process_name_size);
        strcpy(process_name, "5G_ARM");
        
        arm_pid = getpid();
        current_process = ARM;

        signal(SIGTERM, signal_handler); // Only exit when receiving SIGTERM (called by system manager)
        signal(SIGINT, SIG_IGN); // Ignore SIGINT

        // Child process
        #ifdef DEBUG
        printf("<ARM>DEBUG# Authorization Requests Manager has PID %d\n", getpid());
        #endif

        write_to_log("PROCESS AUTHORIZATION_REQUEST_MANAGER CREATED");
        
        write_to_log("CREATING AUTH ENGINES");
        create_auth_engines();
        write_to_log("CREATED AUTH ENGINES");


        create_pipes();
        create_fifo_queues();


        // Create receiver and sender threads
        #ifdef DEBUG
        printf("<ARM>DEBUG# Creating receiver and sender threads...\n");
        #endif
        pthread_create(&receiver_t, NULL, receiver_thread, NULL);
        pthread_create(&sender_t, NULL, sender_thread, NULL);

        #ifdef DEBUG
        printf("<ARM>DEBUG# Threads created successfully\n");
        #endif

        // Wait for threads to finish
        // pthread_join(receiver_t, NULL);
        // pthread_join(sender_t, NULL);

        while(wait(NULL) > 0);

        exit(0);
    }

    // Parent process
    return 0;
}   

// Creates the message queue
int create_message_queue(){
    // Generate a key based on the file name
    
    // Create temporary file
    FILE* temp = fopen(MESSAGE_QUEUE_KEY, "w");
    if(temp == NULL){
        write_to_log("<ERROR CREATING MESSAGE QUEUE KEY>");
        return 1;
    }
    fclose(temp);

    // Generate a key
    key_t queue_key = ftok(MESSAGE_QUEUE_KEY, 'a');
    if((message_queue_id = msgget(queue_key, IPC_CREAT | 0777)) == -1){
        perror("msgget");
        write_to_log("<ERROR CREATING MESSAGE QUEUE>");
        return 1;
    }
    return 0;
}

// Creates the initial auth engines
int create_auth_engines(){
    auth_engine_pids = (pid_t*) malloc(sizeof(pid_t) * config->AUTH_SERVERS);
    auth_engine_pipes = (int**) malloc(sizeof(int*) * (config->AUTH_SERVERS + 1)); // +1 for the extra engine
    if(auth_engine_pipes == NULL){
        write_to_log("<ERROR CREATING AUTH ENGINE PIPES>");
        return 1;
    }

    #ifdef DEBUG
    printf("<ARM>DEBUG# Creating auth engines...\n");
    #endif

    for(int i = 0; i < config->AUTH_SERVERS; i++){
        // Open pipe before forking
        auth_engine_pipes[i] = (int*) malloc(sizeof(int) * 2);

        if(pipe(auth_engine_pipes[i]) == -1){
            write_to_log("<ERROR CREATING AUTH ENGINE PIPE>");
            return 1;
        }

        auth_engine_pids[i] = fork();
        //pid_t engine_pid = fork();
        if(auth_engine_pids[i] == -1){
            write_to_log("<ERROR CREATING AUTH ENGINE>");
            return 1;
        }

        if(auth_engine_pids[i] == 0){
            signal(SIGINT, SIG_IGN);
            signal(SIGTERM, signal_handler);

            // Set process name
            memset(process_name, 0, process_name_size);
            char process_str[40];
            sprintf(process_str, "5G_AUTH_ENGINE%d", i);
            strcpy(process_name, process_str);

            arm_pid = getppid();
            current_process = AUTH_ENGINE;

            // Auth engine
            close(auth_engine_pipes[i][1]); // Close write end of the pipe

            auth_engine_process(i);
            
            #ifdef DEBUG
            printf("<AE%d>DEBUG# Auth engine with PID %d exited\n", i, getpid());
            #endif

            exit(0); // Exit after receiving SIGTERM and processing the last request
        }
        else{
            // Parent process (ARM)
            close(auth_engine_pipes[i][0]); // Close read end of the pipe
        }
    }
    return 0; 
}

// Creates and opens USER_PIPE and BACK_PIPE, called by the ARM
int create_pipes(){
    #ifdef DEBUG
    printf("<ARM>DEBUG# Creating named pipes...\n");
    #endif
    // Unlink pipes if they already exist
    unlink(USER_PIPE);
    unlink(BACK_PIPE);        
    if(mkfifo(USER_PIPE, O_CREAT | O_EXCL | 0666) == -1){
        write_to_log("<ERROR CREATING USER PIPE>");
        exit(1);
    }
    if(mkfifo(BACK_PIPE, O_CREAT | O_EXCL | 0666) == -1){
        write_to_log("<ERROR CREATING BACK PIPE>");
        exit(1);
    }

    #ifdef DEBUG
    printf("<ARM>DEBUG# Opening pipes...\n");
    #endif
    if((fd_user_pipe = open(USER_PIPE, O_RDWR)) == -1){
        write_to_log("<ERROR OPENING USER PIPE>");
        exit(1);
    }
    if((fd_back_pipe = open(BACK_PIPE, O_RDWR)) == -1){
        write_to_log("<ERROR OPENING BACK PIPE>");
        exit(1);
    }

    #ifdef DEBUG
    printf("<ARM>DEBUG# Pipes created and opened successfully\n");
    #endif

    return 0;
}
