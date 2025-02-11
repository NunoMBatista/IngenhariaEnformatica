/*
  Authors: 
    Nuno Batista uc2022216127
    Miguel Castela uc2022212972
*/
#ifndef GLOBAL_H
#define GLOBAL_H

//#define SLOWMOTION 100 // Comment this line to remove slow motion, it's value is the delta coefficient
//#define DEBUG // Comment this line to remove debug messages
//#define QUEUE_PROGRESS_BAR // Comment this line to remove video and other queues progress bar
#define SHARED_MEMORY_DISPLAY // Comment this line to remove shared memory display

#define MAIN_LOCKFILE "/tmp/main_lockfile.lock"
#define BACKOFFICE_LOCKFILE "/tmp/backoffice_lockfile.lock"

#define LOG_SEMAPHORE "log_semaphore"
#define SHARED_MEMORY_SEMAPHORE "shared_memory_semaphore"
#define AUXILIARY_SHM_SEMAPHORE "auxiliary_shm_semaphore"
#define ENGINES_SEMAPHORE "engines_semaphore"

#define MESSAGE_QUEUE_KEY "message_queue_key"

#define PIPE_BUFFER_SIZE 100
#define USER_PIPE "/tmp/USER_PIPE"
#define BACK_PIPE "/tmp/BACK_PIPE"

#ifndef F_TLOCK
#define F_TLOCK 2
#endif

#define max(a, b) ((a) > (b) ? (a) : (b))

#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>   
#include <signal.h>

#include "queue.h"

// Used to easily differentiate stdout messages by color
enum process_type{ 
    SYSMAN, // YELLOW
    ARM, // BLUE
    AUTH_ENGINE, // PURPLE 
    MONITOR_ENGINE // CYAN
};

typedef struct{
    long type;
    char text[PIPE_BUFFER_SIZE];
} QueueMessage;

typedef struct{
    int isActive;
    pid_t user_id;
    int initial_plafond;
    int spent_plafond;
    /*
        The already_notified variable starts at 0, when notified for 80% it becomes 80,
        when notified for 90% it becomes 90 and when notified for 100% it becomes 100.

        A user is only notified if the already_notified variable is less than the notification value
    */
    int already_notified; 
} MobileUserData;

typedef struct{
    int num_users;
    int spent_video;
    int spent_music;
    int spent_social;

    int reqs_video;
    int reqs_music;
    int reqs_social;
    MobileUserData *users;
} SharedMemory; 


// MIGHT REMOVE NESTED SHARED MEMORY AND JUST HAVE AN ARRAY OF ACTIVE AUTH ENGINES AS AUX SHM
typedef struct{
    pthread_mutex_t monitor_engine_mutex;
    pthread_cond_t monitor_engine_cond;

    pthread_mutex_t log_mutex;

    int *active_auth_engines;
} AuxiliaryShm;

typedef struct{
    int MOBILE_USERS; // Max number of users
    int QUEUE_POS; // Number of queue slots
    int AUTH_SERVERS; // Max number of auth engines
    int AUTH_PROC_TIME; // Time taken by auth engines to process requests
    int MAX_VIDEO_WAIT; // Max time that a video request can wait before being processed
    int MAX_OTHERS_WAIT; // Max time that a non-video request can wait before being processed
} Config;

// External declaration to be used in other files
extern Config *config;

//extern MobileUserData* shared_memory;
extern SharedMemory *shared_memory;
extern int shm_id; // General shared memory id
extern int shm_id_users; // Shared memory id for users
extern sem_t *shared_memory_sem;

extern AuxiliaryShm *auxiliary_shm;
extern int aux_shm_id;
extern int engines_shm_id;
extern sem_t *engines_sem; // Semaphore with value config->AUTH_SERVERS to control the number of active auth engines
extern sem_t *aux_shm_sem; // Binary semaphore to control access to auxiliary shared memory
extern pthread_mutexattr_t shared_mutex;
extern pthread_condattr_t shared_cond;
extern pthread_mutexattr_t log_mutex_attr;



extern sem_t* log_semaphore;

extern int **auth_engine_pipes; // Array of pipes fd for the sender to communicate with the auth engines
extern int fd_user_pipe;
extern int fd_back_pipe;

extern Queue *video_queue;
extern Queue *other_queue;
extern int message_queue_id;

extern pthread_mutex_t queues_mutex; 
extern pthread_cond_t sender_cond;

extern volatile int extra_auth_engine; // 0 if it's not active, 1 otherwise
extern pid_t extra_auth_pid; // PID of the extra auth engine

extern pid_t parent_pid; // PID of the parent process
extern pid_t arm_pid; // PID of the ARM process
extern pid_t monitor_pid; // PID of the monitor engine process

extern pthread_t receiver_t;
extern pthread_t sender_t;
extern volatile sig_atomic_t arm_threads_exit; // 1 if the arm threads should exit, 0 otherwise

extern volatile int monitor_exit; // 1 if the monitor engine should exit, 0 otherwise

extern volatile sig_atomic_t auth_engine_exit;
extern pid_t *auth_engine_pids; // Array with the pids of the main auth engines (not the extra one)
extern int auth_engine_index; // Index of the current auth engine (changes between different auth engines) used in kill_auth_engine

extern pthread_t periodic_notifications_t;

extern enum process_type current_process;

extern int process_name_size; 
extern char* process_name;

extern FILE* log_file;

extern int log_mutex_initialized;
#endif