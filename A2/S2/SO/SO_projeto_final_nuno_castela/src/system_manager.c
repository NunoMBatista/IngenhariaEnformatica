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
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>

#include "global.h"
#include "system_functions.h"
#include "queue.h"

/*
    Execution instructions:
    ./5g_auth_platform {config_file}
*/

Config* config;
sem_t* log_semaphore;

//MobileUserData* shared_memory;
// Used for main shared memory
SharedMemory *shared_memory;
int shm_id_users;
int shm_id;
sem_t *shared_memory_sem;

// Used for auxiliary shared memory
AuxiliaryShm *auxiliary_shm;
int aux_shm_id;
int engines_shm_id;
sem_t *engines_sem;
sem_t *aux_shm_sem;
pthread_mutexattr_t shared_mutex;
pthread_condattr_t shared_cond;
pthread_mutexattr_t log_mutex_attr;

// Mutex for the log file
pthread_mutex_t log_mutex;

// Pipe file descriptors
int fd_user_pipe; 
int fd_back_pipe;
int **auth_engine_pipes;

// Queues
Queue *video_queue;
Queue *other_queue;
int message_queue_id;

// Mutexes and condition variables
pthread_mutex_t queues_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t sender_cond = PTHREAD_COND_INITIALIZER;

volatile int extra_auth_engine = 0;
pid_t extra_auth_pid = -1;

pid_t parent_pid;
pid_t arm_pid;
pid_t monitor_pid;

pthread_t receiver_t;
pthread_t sender_t;
volatile sig_atomic_t arm_threads_exit = 0;

int process_name_size; 
char* process_name;

pid_t *auth_engine_pids; // Array with the pids of the main auth engines (not the extra one)

enum process_type current_process;

FILE* log_file;

int log_mutex_initialized = 0;

int main(int argc, char *argv[]){
    // Set this process name
    process_name_size = strlen(argv[0]);
    process_name = argv[0];
    memset(process_name, 0, process_name_size);
    strcpy(process_name, "5G_SYSTEM_MANAGER");

    #ifdef DEBUG
    printf("<SYS MAN> Is process number %d\n", getpid());
    #endif
    // Create a lockfile to prevent multiple instances of the program with only read permissions for the group
    int lockfile = open(MAIN_LOCKFILE, O_RDWR | O_CREAT, 0640);
    if (lockfile == -1){
        perror("open");
        return 1;
    }
    // Try to lock the file
    if(lockf(lockfile, F_TLOCK, 0) == -1){
        printf("!!! ANOTHER INSTANCE OF THE PROGRAM IS ALREADY RUNNING !!!\n");
        return 1;
    }

    // Check correct number of arguments
    if(argc != 2){
        printf("<INCORRECT NUMBER OF ARGUMENTS>\n Correct usage: %s {config_file}\n", argv[0]);
        return 1;
    }

    parent_pid = getpid();
    current_process = SYSMAN;

    char *config_file = argv[1];

    log_file = fopen("log.txt", "a");
    if(log_file == NULL){
        perror("PROBLEM OPENING LOG FILE\n");
        return 1;
    }

    if(initialize_system(config_file) != 0){
        write_to_log("<SYSTEM INITIALIZATION FAILED>\n");
        return 1;
    };

    // Wait for all child processes to finish
    while(wait(NULL) > 0);

    clean_up();
    return 0; 
}