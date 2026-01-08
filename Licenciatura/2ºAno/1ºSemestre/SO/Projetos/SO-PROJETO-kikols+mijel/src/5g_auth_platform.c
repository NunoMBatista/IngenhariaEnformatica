/* 
 * Versão de 2ªMeta da disciplina Sistemas Operativos
 * 
 * TRABALHO REALIZADO POR:
 *      Francisco Amado Lapa Marques Silva - 2022213583
 *      Miguel Moital Rodrigues Cabral Martins - 2022213951
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <time.h>
#include <sys/select.h>

#include <sys/stat.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <errno.h>

#include "message_struct.c"
#include "queue_struct.c"


#define MAX(a,b) (((a)>(b))?(a):(b))


#define LOG_FILE "log.txt"

#define MOBILE_PIPE "/tmp/mobile_pipe"
#define BACKEND_PIPE "/tmp/backend_pipe"

#define BUF_SIZE 1024
#define REQ_SIZE 64
#define MESSAGE_QUEUE 1234


// Verify that "settings"' values are valid
int validate_settings();

// Write to log, and read from .config file
int append_logfile(char *log_info);
int read_configfile(char *configfile);

// Handle SIGINT signal (^C), closing all processes cleanly, and freeing shared memory
void system_panic();
void close_system_manager(int sigint);
void close_monitor_engine();
void close_authorization_request_manager();
void close_authorization_engine();

// Create shared memory
int create_usershmem();
int create_AEsshmem();
int create_MEshmem();

/* Creates child proccesses parallel to the parent proccess
 * parallel_AuthorizationRequestManager creates ARM
 * parallel_AuthorizationEngine creates AE
 * parallel_MonitorEngine creates ME
 */
int parallel_AuthorizationRequestManager();
int parallel_AuthorizationEngine(int n);
int parallel_MonitorEngine();

/* Authorization_Request_Manager's threads.
 * RECEIVER: Used to recieve requests from mobile clients
 * SENDER: Used to send to Authorization_Engine's
 */
void *receiver_ARM( void *arg );
void *sender_ARM( void *arg );
void *sender_ME();

/* Utils */
int check_requesttype(char *request);
int kill_allchildren(int ARM_flag);
int handle_request(int id, char *request, char* response);

// client stuff
int check_plafond(int pid, int data);
int create_client(int pid, int plafond);
int delete_client(int pid, int perm_flag);
int reset_stats();
int fetch_stats(int *request_count, int *data_count);



typedef struct Settings {
    int MOBILE_USERS;       // número de Mobile Users que serão suportados pelo simulador
    int QUEUE_POS;          // número de slots nas filas que são utilizadas para armazenar os pedidos de autorização e os comandos dos utilizadores (>=0)
    int AUTH_SERVERS;       // número de Authorization Engines que devem ser lançados inicialmente (>=1)
    int AUTH_PROC_TIME;     // período (em ms) que o Authorization Engine demora para processar os pedidos

    int MAX_VIDEO_WAIT;     // tempo máximo (em ms) que os pedidos de autorização do serviço de vídeo podem aguardar para serem executados (>=1)
    int MAX_OTHERS_WAIT;    // tempo máximo (em ms) que os pedidos de autorização dos serviços de música e de redes sociais, bem como os comandos podem aguardar para serem executados (>=1)
} Settings;

typedef struct User_data {
    int id;                     // pid of mobile user
    int flag;                   // flag to check if plaffond has to be reported
    int init_plafond;           // initial plafond
    int plafond_left;           // plafond left ( >= 0 disconnect )

    int request_count;          // number of requests made  } can be used by stats and reset
    int data_count;             // number of data used      }
} User_data;

typedef struct Monitor_stuff {
    pthread_mutex_t ME_lock;
    pthread_cond_t ME_cond;
} Monitor_stuff;



// General stuff
sem_t *log_sem;
struct Settings settings;
int pid;
int system_manager_pid;
int running = 1;

// Shared memory stuff
sem_t *user_sem;        // }
int user_shmid;         // } used to handle users
User_data *user_array;  // }
sem_t *AEs_sem;             // }
int AEs_shmid;              // } used to handle AEs, 0 means ready, 1 means busy
int *AEs_array;             // }
int monitor_shmid;              // } used to handle the monitor engine
Monitor_stuff *monitor_stuff;   // }
sem_t *ME_sem;                  // }


// Store child pids of the given process
int child_count;
int *child_pids;
int (*AE_unpipes)[2];     // only for ARM

// Message queue
int message_queue_id;

// process name stuff
int max_processname_size;
char *process_name;

FILE *logfd;


int main(int argc, char *argv[]) {
    system_manager_pid = getpid();
    process_name = argv[0];
    max_processname_size = strlen(process_name);

    signal(SIGINT, close_system_manager);           // redirect ^C (ctrl+c) command
    signal(SIGQUIT, close_system_manager);          // redirect ^\ (ctrl+\) command, and handle panic
    /* Verify correct amount of arguments */
    if (argc!=2) {
        fprintf(stderr, "!!!INCORRECT ARGUMENTS!!!\n-> %s {config-file}\n", argv[0]);
        exit(1);
    }

    
    /* Read config file, and set "settings" */
    if (read_configfile(argv[1])!=0) {
        exit(1);
    }

    /* Verify that settings on config file are valid */
    if ( validate_settings()!=0 ) {
        exit(1);
    }

    /* Create "child_pids" with size settings.AUTH_SERVERS+1 or 2, depending who is bigger 
     * 2 because system_manager needs 2 children */
    child_count = 0;
    child_pids = (int*) malloc(sizeof(int) * MAX(settings.AUTH_SERVERS+1,2));
    memset(child_pids, -1, sizeof(int) * MAX(settings.AUTH_SERVERS+1,2));

    /* Create message queue */
    message_queue_id = msgget(MESSAGE_QUEUE, IPC_CREAT | 0777);

    /* Create semaphore for log file */
    log_sem = sem_open("log_sem", O_CREAT, 0777, 1);
    if ( log_sem==SEM_FAILED ) {
        fprintf(stderr, "ERROR CAN'T CREATE LOG SEMAPHORE\n");
        system_panic(-1);
    }

    /* Create semaphore for shared memory */
    user_sem = sem_open("user_sem", O_CREAT, 0777, 1);
    if ( user_sem==SEM_FAILED ) {
        fprintf(stderr, "ERROR CAN'T CREATE SHM SEMAPHORE\n");
        system_panic(-1);
    }

    /* Create semaphore for ME */
    ME_sem = sem_open("ME_sem", O_CREAT, 0777, 0);
    if ( ME_sem==SEM_FAILED ) {
        fprintf(stderr, "ERROR CAN'T CREATE SHM SEMAPHORE FOR ME\n");
        system_panic(-1);
    }

    /* Create user shared memory */
    if ( create_usershmem()!=0 ) {
        fprintf(stderr, "ERROR CAN'T CREATE SHARED MEMORY\n");
        system_panic(-1);
    }

    int reset_stats();


    /* Open log FILE */
    logfd = fopen(LOG_FILE, "a");
    if (logfd==NULL) {
        // Could not open file
        fprintf(stderr, "[ERROR] Could not open file \"%s\".\n", LOG_FILE);
        exit(1);
    }
    

    append_logfile("5G_AUTH_PLATFORM SIMULATOR STARTING");
    append_logfile("PROCESS SYSTEM_MANAGER CREATED");

    memset(process_name, '\0', max_processname_size);   // }
    strcpy(process_name, "SYSTEM_MANAGER");             // } change process name to SYSTEM_MAN

    // Create child proccesses of the system
    parallel_AuthorizationRequestManager();
    parallel_MonitorEngine();

    wait(NULL);
    return 0;
}



int validate_settings() {
    /* Returns 0 if "settings" is up to code;
     * Returns 1 if "settings" is wrongly configured, and prints to stderr what is wrong
     */
    int flag = 0;
    if (settings.MOBILE_USERS <= 0) {
        fprintf(stderr, "[ERROR_CONFIG]: {MOBILE_USERS} must be >= 1\n");
        flag = 1;
    }
    if (settings.QUEUE_POS <= 0) {
        fprintf(stderr, "[ERROR_CONFIG]: {QUEUE_POS} must be >= 1\n");
        flag = 1;
    }
    if (settings.AUTH_SERVERS <= 0) {
        fprintf(stderr, "[ERROR_CONFIG]: {AUTH_SERVERS} must be >= 1\n");
        flag = 1;
    }
    if (settings.AUTH_PROC_TIME < 0) {
        fprintf(stderr, "[ERROR_CONFIG]: {AUTH_PROC_TIME} must be >= 0\n");
        flag = 1;
    }
    if (settings.MAX_VIDEO_WAIT <= 0) {
        fprintf(stderr, "[ERROR_CONFIG]: {MAX_VIDEO_WAIT} must be >= 1\n");
        flag = 1;
    }
    if (settings.MAX_OTHERS_WAIT <= 0) {
        fprintf(stderr, "[ERROR_CONFIG]: {MAX_OTHERS_WAIT} must be >= 1\n");
        flag = 1;
    }
    return flag;
}



void system_panic() {
    /* Can be called by any process to make SYSTEM_MANAGER panic, and stop all operations */
    kill(system_manager_pid, SIGQUIT);
    while (1) { sleep(1); }     // wait to be killed
}

void close_system_manager(int sigint) {
    /* Used to close SYSTEM_MANAGER (and all child processes) */
    if (sigint==SIGINT) {
        append_logfile("SIGNAL SIGINT RECEIVED");
    } else if (sigint==SIGQUIT) {
        append_logfile("SIGQUIT (possibly panic) SIGNAL RECEIVED");
    } else {
        append_logfile("UNKNOWN SIGNAL RECEIVED");
    }
    
    append_logfile("5G_AUTH_PLATFORM SIMULATOR WAITING FOR LAST TASKS TO FINISH");
    kill_allchildren(0);    // kill all the children of this process

    /* Close message queue */
    msgctl(message_queue_id, IPC_RMID, 0);

    append_logfile("5G_AUTH_PLATFORM SIMULATOR CLOSING\n"
                   "+----------------------------------------------------------------------+");

    fclose(logfd);      // close logfile

    sem_close(log_sem);         // }
    sem_unlink("log_sem");      // } unlink and close log_sem
    sem_close(user_sem);            // }
    sem_unlink("user_sem");         // } unlink and close user_sem
    sem_close(ME_sem);
    sem_unlink("ME_sem");
    shmdt(user_array);                  // }
    shmctl(user_shmid, IPC_RMID, 0);    // } free shared memory

    exit(0);
}

void close_monitor_engine() {
    /* Used to close MONITOR_ENGINE */
    append_logfile("MONITOR_ENGINE CLOSING");

    exit(0);
}

void close_authorization_request_manager() {
    /* Used to close AUTHORIZATION_REQUEST_MANAGER */
    append_logfile("AUTHORIZATION_REQUEST_MANAGER WAITING FOR LAST TASKS TO FINISH");

    kill_allchildren(1);    // kill all the children of this process

    // close named pipes
    unlink(BACKEND_PIPE);
    unlink(MOBILE_PIPE);

    // close unnamed pipes
    for (int i=0; i<settings.AUTH_SERVERS+1; i++) {
        if (child_pids[i]==-1) {
            break;
        }
        close(AE_unpipes[i][0]);
        close(AE_unpipes[i][1]);
    }

    sem_close(AEs_sem);         // }
    sem_unlink("AEs_sem");      // } unlink and close AEs_sem
    shmdt(AEs_array);               // }
    shmctl(AEs_shmid, IPC_RMID, 0); // } free shared memory

    append_logfile("AUTHORIZATION_REQUEST_MANAGER CLOSING");

    exit(0);
}

void close_authorization_engine() {
    /* Used to close AUTHORIZATION_ENGINE */
    append_logfile("AUTHORIZATION_ENGINE CLOSING");

    running = 0;    // stop the while loop in AE, so that AE can exit only when it finishes what it's doing
}



int create_usershmem() {
    /* Create sharedmemory (user_array) */
    user_shmid = shmget(IPC_PRIVATE, sizeof(User_data) * settings.MOBILE_USERS, IPC_CREAT | 0777);
    if (user_shmid == -1) {
        fprintf(stderr, "[ERROR] Could not create shared memory\n");
        return 1;
    }
    user_array = (User_data*) shmat(user_shmid, NULL, 0);
    if (user_array == (void*)-1) {
        fprintf(stderr, "[ERROR] Could not assign shared memory\n");
        return 1;
    }

    // set user_array to default value
    sem_wait(user_sem);
    for (int i=0; i<settings.MOBILE_USERS; i++) {
        user_array[i].id = -1;
        user_array[i].plafond_left = -1;
    }
    sem_post(user_sem);

    return 0;
}

int create_AEsshmem() {
    /* Create sharedmemory (AEs_array) */
    AEs_shmid = shmget(IPC_PRIVATE, sizeof(int) * settings.AUTH_SERVERS, IPC_CREAT | 0777);
    if (AEs_shmid == -1) {
        fprintf(stderr, "[ERROR] Could not create shared memory\n");
        return 1;
    }
    AEs_array = (int*) shmat(AEs_shmid, NULL, 0);
    if (AEs_array == (void*)-1) {
        fprintf(stderr, "[ERROR] Could not assign shared memory\n");
        return 1;
    }

    // set AEs_array to default value
    sem_wait(AEs_sem);
    for (int i=0; i<settings.AUTH_SERVERS+1; i++) {
        AEs_array[i] = 0;
    }
    sem_post(AEs_sem);

    return 0;
}

int create_MEshmem() {
    /* Create sharedmemory (monitor_stuff) */
    monitor_shmid = shmget(IPC_PRIVATE, sizeof(Monitor_stuff), IPC_CREAT | 0777);
    if (monitor_shmid == -1) {
        fprintf(stderr, "[ERROR] Could not create shared memory\n");
        return 1;
    }
    monitor_stuff = (Monitor_stuff*) shmat(monitor_shmid, NULL, 0);
    if (monitor_stuff == (void*)-1) {
        fprintf(stderr, "[ERROR] Could not assign shared memory\n");
        return 1;
    }

    // initialize monitor_stuff
    pthread_mutex_init(&(monitor_stuff->ME_lock), NULL);
    pthread_cond_init(&(monitor_stuff->ME_cond), NULL);

    return 0;

}



int append_logfile(char *log_info) {
    sem_wait(log_sem);
    //FILE *logf = fopen(LOG_FILE, "a");
    if (logfd==NULL) {
        // Could not open file
        fprintf(stderr, "[ERROR] Could not open file \"%s\".\n", LOG_FILE);
        // Don't log error, obviously
        return 1;
    }

    time_t posix_time;
    struct tm* time_struct;
    char time_buffer[80];

    time(&posix_time);      // Get time since epoch
    time_struct = localtime(&posix_time);

    strftime(time_buffer, 80, "%X", time_struct);       // write time hour:min:sec
    fprintf(logfd,  "%s %s\n", time_buffer, log_info);      // } Write log info to log.txt
    fprintf(stdout, "%s %s\n", time_buffer, log_info);      // } and to terminal

    //fclose(logf);
    sem_post(log_sem);
    return 0;
}

int read_configfile(char *configfile) {
    /* Read from filepath "configfile", and write to global Settings "settings" */
    FILE *file = fopen(configfile, "r");
    if (file==NULL) {
        // could not open file
        fprintf(stderr, "[ERROR] Could not open config file \"%s\".\n", configfile);
        return 1;
    }

    if ( fscanf(file, "%d\n%d\n%d\n%d\n%d\n%d", &settings.MOBILE_USERS, &settings.QUEUE_POS, &settings.AUTH_SERVERS, &settings.AUTH_PROC_TIME, &settings.MAX_VIDEO_WAIT, &settings.MAX_OTHERS_WAIT)!=6 ) {
        // if fscanf does not read 6 numbers, fail, incorrect format
        fprintf(stderr, "[ERROR] File \"%s\" is not formated correctly.\n", configfile);
        return 1;
    }

    // DEBUG show settings
    fprintf( stdout, "[DEBUG] Settings:\n\tMOBILE_USERS->%d\n\tQUEUE_POS->%d\n\tAUTH_SERVERS->%d\n\tAUTH_PROC_TIME->%d\n\tMAX_VIDEO_WAIT->%d\n\tMAX_OTHERS_WAIT->%d\n", settings.MOBILE_USERS, settings.QUEUE_POS, settings.AUTH_SERVERS, settings.AUTH_PROC_TIME, settings.MAX_VIDEO_WAIT, settings.MAX_OTHERS_WAIT );

    fclose(file);
    return 0;
}



int parallel_AuthorizationRequestManager() {
    /* Creates a new process that will have the role of AUTHORIZATION_REQUEST_MANAGER */
    pid = fork();
    if (pid<0) {
        // fork failed;
        append_logfile("[ERROR] FAILED TO CREATE PROCESS AUTHORIZATION_REQUEST_MANAGER");
        return 1;
    } else if (pid!=0) {
        // if pid=0, parent process, save child pid to child_pids and return
        child_pids[child_count] = pid;
        child_count++;
        return 0;
    }

    /* if pid=0, child process, now authorization request manager */
    memset(process_name, '\0', max_processname_size);       // }
    strcpy(process_name, "AUTH_REQ_MANAGER");               // } change process name to ARM

    child_count = 0;                                                        // } new process, no children
    memset(child_pids, -1, sizeof(int) * MAX(settings.AUTH_SERVERS+1,2));   // }

    AE_unpipes = (int(*)[2])malloc(sizeof(int[2]) * settings.AUTH_SERVERS+1);

    //char logbuffer[BUF_SIZE];

    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, close_authorization_request_manager);

    pthread_t reciever, sender;
    append_logfile("PROCESS AUTHORIZATION_REQUEST_MANAGER CREATED");

    // Create semaphore for AEs
    AEs_sem = sem_open("AEs_sem", O_CREAT, 0777, 1);
    if ( AEs_sem==SEM_FAILED ) {
        append_logfile("[ERROR] COULD NOT CREATE SHM SEMAPHORE FOR AE's");
        system_panic();
    }

    // Create shared memory for AEs
    if ( create_AEsshmem()!=0 ) {
        append_logfile("[ERROR] COULD NOT CREATE SHARED MEMORY FOR AE's");
        system_panic();
    }

    // Create queues
    queue q[2];
    pthread_cond_t written = PTHREAD_COND_INITIALIZER, state = PTHREAD_COND_INITIALIZER;
    pthread_mutex_t written_lock = PTHREAD_MUTEX_INITIALIZER, state_lock = PTHREAD_MUTEX_INITIALIZER;
    int state_var = 0;
    create_queue(&q[0], settings.QUEUE_POS, &written, &written_lock, &state_var, &state, &state_lock);
    create_queue(&q[1], settings.QUEUE_POS, &written, &written_lock, &state_var, &state, &state_lock);

    
    for (int i=0; i<settings.AUTH_SERVERS; i++) {
        // create unnamed pipes for each AE, and create AEs
        if (pipe(AE_unpipes[i])<0) {
            append_logfile("[ERROR] COULD NOT CREATE UNNAMED PIPE FOR AE\n");
            system_panic();
        }
        parallel_AuthorizationEngine(i+1);
    }

    pthread_create(&reciever, NULL, receiver_ARM, q);
    pthread_create(&sender, NULL, sender_ARM, q);

    // Handle extra AE creation
    pthread_mutex_lock(q[0].statecond_lock);
    int extra_AE_pid = -1;
    while (1) {
        pthread_cond_wait(q[0].statecond, q[0].statecond_lock);
        //sprintf(logbuffer, "STATECOND SIGNAL RECEIVED, signal=%d\n", *q[0].state);
        //append_logfile(logbuffer);
        if (*q[0].state==1) {
            // create extra AE
            if (child_pids[settings.AUTH_SERVERS]!=-1) {
                append_logfile("[ERROR] EXTRA AUTHORIZATION_ENGINE ALREADY EXISTS, PANIC!");
                system_panic();
            }
            if (pipe(AE_unpipes[settings.AUTH_SERVERS])<0) {
                append_logfile("[ERROR] COULD NOT CREATE UNNAMED PIPE FOR EXTRA AE\n");
                system_panic();
            }
            parallel_AuthorizationEngine(settings.AUTH_SERVERS+1);
            append_logfile("EXTRA AUTHORIZATION_ENGINE CREATED");
        } else {
            // destroy extra AE
            extra_AE_pid = child_pids[settings.AUTH_SERVERS];
            if (extra_AE_pid==-1) {
                append_logfile("[ERROR] TRYING TO KILL NON-EXISTENT EXTRA AE, PANIC! (i thought i fixed this D: )");
                system_panic();
            }
            child_pids[settings.AUTH_SERVERS] = -1;     // } mark as dead so no more requests come in
            child_count--;                              // }
            kill(extra_AE_pid, SIGQUIT);
            write(AE_unpipes[settings.AUTH_SERVERS][1], "-1#", 4);  // dummy request to flush AE pipes
            waitpid(extra_AE_pid, NULL, 0);
            close(AE_unpipes[settings.AUTH_SERVERS][0]);        // }
            close(AE_unpipes[settings.AUTH_SERVERS][1]);        // } close extra AE pipes
            append_logfile("EXTRA AUTHORIZATION_ENGINE DESTROYED");
        }
    }
    pthread_mutex_unlock(q[0].statecond_lock);

    pthread_join(reciever, NULL);
    pthread_join(sender, NULL);

    exit(0);
}

int parallel_AuthorizationEngine(int n) {
    /* Creates a new process that will have the role of AUTHORIZATION_ENGINE */
    pid = fork();
    if (pid<0) {
        // fork failed;
        append_logfile("[ERROR] FAILED TO CREATE PROCESS AUTHORIZATION_ENGINE");
        return 1;
    } else if (pid!=0) {
        // if pid=0, parent process, save child pid to child_pids and return
        AEs_array[n-1] = 0;
        child_pids[child_count] = pid;
        child_count++;
        return 0;
    }
    memset(process_name, '\0', max_processname_size);   // }
    sprintf(process_name, "AUTH_ENGINE%d", n);          // } change process name to AEn

    child_count = 0;                                                        // } new process, no children
    memset(child_pids, -1, sizeof(int) * MAX(settings.AUTH_SERVERS+1,2));   // }

    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, close_authorization_engine);


    // if pid=0, child process, now authorization engine
    int id = n;
    char log_message[BUF_SIZE];
    sprintf(log_message, "PROCESS AUTHORIZATION_ENGINE %d CREATED", id);
    append_logfile(log_message);

    int read_n;
    char request[BUF_SIZE], response[BUF_SIZE];
    struct message msg; int cli_pid;
    while (running==1) {        // "running" is to make the AE not exit during an operation
        // read from unnamed pipe AE_unpipes[n-1][0]
        read_n = read(AE_unpipes[id-1][0], request, REQ_SIZE);
        //printf("recieved %d bytes\n", read_n);
        if (read_n==0) {
            // if read returns 0, pipe is closed, exit
            break;
        }
        //printf("[AE %d] %s\n", id, request);
        cli_pid = handle_request(id, request, response);
        usleep(1000*settings.AUTH_PROC_TIME);    // sleep for AUTH_PROC_TIME millis
        if (response[0]!='\0' && cli_pid!=-1) {
            // if response is not NULL, send response to client
            //printf("[AE %d] RESPONDING TO %d-\"%s\"\n", id, cli_pid, response);
            msg.mtype = cli_pid;
            strcpy(msg.mtext, response);
            msgsnd(message_queue_id, &msg, sizeof(msg), 0);
        }
        sem_wait(AEs_sem);
        AEs_array[id-1] = 0;
        sem_post(AEs_sem);
    }
    exit(0);    // AE exits on it's own terms
}

int parallel_MonitorEngine() {
    /* Creates a new process that will have the role of MONITOR_ENGINE */
    pid = fork();
    if (pid<0) {
        // fork failed;
        append_logfile("[ERROR] FAILED TO CREATE PROCESS MONITOR_ENGINE");
        return 1;
    } else if (pid!=0) {
        // if pid=0, parent process, save child pid to child_pids and return
        child_pids[child_count] = pid;
        child_count++;
        return 0;
    }

    /* if pid=0, child process, now monitor engine */
    memset(process_name, '\0', max_processname_size);   // }
    strcpy(process_name, "MONITOR_ENGINE");             // } change process name to ME

    child_count = 0;                                                        // } new process, no children
    memset(child_pids, -1, sizeof(int) * MAX(settings.AUTH_SERVERS+1,2));   // }

    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, close_monitor_engine);

    int plafond_spend, plafond_init;
    double plafond_percent;
    message msg;

    append_logfile("PROCESS MONITOR_ENGINE CREATED");

    pthread_t sender;
    pthread_create(&sender, NULL, sender_ME, NULL);

    //pthread_mutex_lock(&(monitor_stuff->ME_lock));
    while (1) {
        //pthread_cond_wait(&(monitor_stuff->ME_cond), &(monitor_stuff->ME_lock));
        sem_wait(ME_sem);

        sem_wait(user_sem);
        for (int i=0; i<settings.MOBILE_USERS; i++) {
            if (user_array[i].id!=-1 && user_array[i].flag==1) {
                // if user is connected, and has flag to check plafond
                plafond_init = user_array[i].init_plafond;
                plafond_spend = plafond_init - user_array[i].plafond_left;
                plafond_percent = (double)plafond_spend/(double)plafond_init;   // calculate plafond percentage spent

                msg.mtype = user_array[i].id;       // so the message will reach the correct user
                if ( plafond_percent >= 1 ) {
                    // if plafond is spent, signal to user
                    append_logfile("NOTIFYING USER OF DISCONNECT, PLAFOND 100% SPENT");
                    sprintf(msg.mtext, "Plafond is 100%% spent, disconnecting...");
                    msgsnd(message_queue_id, &msg, sizeof(msg), 0);
                    sprintf(msg.mtext, "DISCONNECT");
                    msgsnd(message_queue_id, &msg, sizeof(msg), 0);
                    delete_client(user_array[i].id, 1);

                } else if ( plafond_percent >= 0.9 ) {
                    // if plafond is 80% spent, signal to user
                    sprintf(msg.mtext, "Plafond is 90%% spent, be careful...");
                    msgsnd(message_queue_id, &msg, sizeof(msg), 0);
                    append_logfile("NOTIFYING USER OF PLAFOND 90% SPENT");

                } else if ( plafond_percent >= 0.8 ) {
                    // if plafond is 50% spent, signal to user
                    sprintf(msg.mtext, "Plafond is 80%% spent, be careful...");
                    msgsnd(message_queue_id, &msg, sizeof(msg), 0);
                    append_logfile("NOTIFYING USER OF PLAFOND 80% SPENT");

                }
                // else, do nothing
                user_array[i].flag = 0;     // reset flag
            }

        }
        sem_post(user_sem);
    }
    
    pthread_join(sender, NULL);
    //pthread_mutex_unlock(&(monitor_stuff->ME_lock));
    exit(0);
}



void *receiver_ARM( void *arg ) {
    /* Reciever (AUTHORIZATION_REQUEST_MANAGER) */
    char request[REQ_SIZE];
    char logbuffer[BUF_SIZE];
    int mobile_pipe_fd, backend_pipe_fd;

    append_logfile("THREAD RECEIVER CREATED");
    
    queue *video_queue = (queue *)arg;
    queue *others_queue = (queue *)arg+1;


    if ( (mkfifo(BACKEND_PIPE, O_CREAT|O_EXCL|0600)<0) && (errno!=EEXIST) ) {
        // Creates the BACKEND named pipe if it doesn't exist yet
        append_logfile("[ERROR] COULD NOT CREATE BACKEND PIPE");
        system_panic();
    } else if ((backend_pipe_fd = open(BACKEND_PIPE, O_RDWR)) < 0) {
        // Opens BACKEND named pipe for reading
        append_logfile("[ERROR] COULD NOT OPEN BACKEND PIPE");
        system_panic();
    } else {
        append_logfile("BACKEND PIPE CREATED");
    }

    if ( (mkfifo(MOBILE_PIPE, O_CREAT|O_EXCL|0600)<0) && (errno!=EEXIST) ) {
        // Creates the BACKEND named pipe if it doesn't exist yet
        append_logfile("[ERROR] COULD NOT CREATE MOBILEUSER PIPE");
        system_panic();
    } else if ((mobile_pipe_fd = open(MOBILE_PIPE, O_RDWR)) < 0) {
        // Opens BACKEND named pipe for reading
        append_logfile("[ERROR] COULD NOT OPEN MOBILEUSER PIPE");
        system_panic();
    } else {
        append_logfile("MOBILEUSER PIPE CREATED");
    }

    fd_set readfds;
    while (1) {
        // Set FD_SET to read from backend_pipe_fd and mobile_pipe_fd
        FD_ZERO(&readfds);
        FD_SET(backend_pipe_fd, &readfds);
        FD_SET(mobile_pipe_fd, &readfds);

        // Wait until something is written to one of the pipes
        if ( select(mobile_pipe_fd+1, &readfds, NULL, NULL, NULL)>0 ) {
            if (FD_ISSET(backend_pipe_fd, &readfds)) {
                // If is written to backend_pipe_fd
                if (read(backend_pipe_fd, &request, sizeof(request))!=EOF) {
                    //printf("[RECEIVED] BACKEND -> %s\n", inbuffer);
                } else {
                    // if read fails, exit
                    append_logfile("[FAILURE] PIPE TO BACKEND BROKEN, PANIC!");
                    system_panic();
                }
            }
            if (FD_ISSET(mobile_pipe_fd, &readfds)) {
                // If is written to mobile_pipe_fd
                if (read(mobile_pipe_fd, &request, sizeof(request))!=EOF) {
                    //printf("[RECEIVED] MOBILE -> %s\n", inbuffer);
                } else {
                    // if read fails, exit
                    append_logfile("[FAILURE] PIPE TO MOBILEUSER BROKEN, PANIC!");
                    system_panic();
                }
            }
            
            /* check request type, and write to right queue */
            if (check_requesttype(request) ) {
                if (write_queue(video_queue, request)!=0) {
                    // video_queue is full, discard
                    sprintf(logbuffer, "[QUEUE_FULL] VIDEO_QUEUE FULL, REQUEST \"%s\" DISCARDED", request);
                    append_logfile(logbuffer);
                }
            } else {
                if (write_queue(others_queue, request)!=0) {
                    // others_queue is full, discard
                    sprintf(logbuffer, "[QUEUE_FULL] OTHERS_QUEUE FULL, REQUEST \"%s\" DISCARDED", request);
                    append_logfile(logbuffer);
                }
            }
        }
    }
    return NULL;
}

void *sender_ARM( void *arg ) {
    /* Sender (AUTHORIZATION_REQUEST_MANAGER) */
    append_logfile("THREAD SENDER CREATED");
    queue *video_queue = (queue *)arg;
    queue *others_queue = (queue *)arg+1;

    char request[REQ_SIZE];
    char logbuffer[BUF_SIZE];
    unsigned long long req_time;

    int next_AE = 0;
    while (1) {
        // Wait for a signal to read from the queues
        pthread_mutex_lock(video_queue->writtencond_lock);
        pthread_cond_wait(video_queue->writtencond, video_queue->writtencond_lock);
        pthread_mutex_unlock(video_queue->writtencond_lock);

        while (1) {
            // read until BOTH queues are empty
            if (count_queue(video_queue)>0) {
                // Priority to video queue
                read_queue(video_queue, request, &req_time);
            } else if (count_queue(others_queue)>0) {
                // If video queue is empty, others queue must have smth
                read_queue(others_queue, request, &req_time);
            } else {
                // If both queues are empty, wait for a signal
                break;
            }

            /* Discard Timed-out requests */
            if (get_time_millis()-req_time > (unsigned long long)settings.MAX_VIDEO_WAIT && check_requesttype(request) ) {
                // if request is video, and has waited too long, discard
                sprintf(logbuffer, "[TIMEOUT] VIDEO REQUEST \"%s\" TIMED OUT WITH %lldms, DISCARDING", request, get_time_millis()-req_time);
                append_logfile(logbuffer);
                continue;
            } else {
                //printf("REQUEST: \"%s\" - timed %ld\n", request, clock()-req_time);
            }
            if (get_time_millis()-req_time > (unsigned long long)settings.MAX_OTHERS_WAIT && !check_requesttype(request) ) {
                // if request is not video, and has waited too long, discard
                sprintf(logbuffer, "[TIMEOUT] VIDEO REQUEST \"%s\" TIMED OUT WITH %lldms, DISCARDING", request, get_time_millis()-req_time);
                append_logfile(logbuffer);
                continue;
            } else {
                //printf("REQUEST: \"%s\" - timed %lld\n", request, get_time_millis()-req_time);
            }

            // Write to first "next" AE that is alive and free
            while (1) {
                if (next_AE>=settings.AUTH_SERVERS+1) {
                    // wrap around child_pids
                    next_AE = 0;
                }
                //printf("TRYING TO SEND TO AE %d with pid %d\n", next_AE, child_pids[next_AE]);
                sem_wait(AEs_sem);
                if (child_pids[next_AE]>0 && AEs_array[next_AE]==0) {
                    // if AE is alive
                    sem_post(AEs_sem);
                    if (write(AE_unpipes[next_AE][1], request, REQ_SIZE) == -1) {
                        // if write fails, AE might be dead, or pipe full
                        sprintf(logbuffer, "[FAILURE] UNPIPE TO AE BROKEN OR FULL %d, SKIPPING", next_AE);
                        append_logfile(logbuffer);
                        //printf("AE %d with pid %d\n", next_AE, child_pids[next_AE]);
                        next_AE++;
                        continue;
                    }
                    sem_wait(AEs_sem);
                    AEs_array[next_AE] = 1;
                    sem_post(AEs_sem);

                    next_AE++;
                    break;
                } else {
                    // if AE is dead, or busy, try next
                    sem_post(AEs_sem);
                }
                next_AE++;
            }

        }
    }

    return NULL;
}

void *sender_ME() {
    /* Sender (MONITOR_ENGINE) */
    message msg;
    char stats[BUF_SIZE];
    int request_count, data_count;
    append_logfile("THREAD SENDER_ME CREATED");

    while(1) {
        sleep(30);
        msg.mtype = 1;
        fetch_stats(&request_count, &data_count);
        sprintf(stats, "Total requests: %d\tTotal data: %d", request_count, data_count);
        strcpy(msg.mtext, stats);
        append_logfile("SENDING SQUEDULED STATS TO BACKOFFICE");
        msgsnd(message_queue_id, &msg, sizeof(msg), 0);        // send message to backoffice broadcast (1)
    }
    return NULL;
}


/* UTILS */
int check_requesttype(char *request) {
    /* Funcao para verificar se a request vai para a queue de video ou de outros */
    char *copy = strdup(request);
    strtok(copy, "#");
    char *token = strtok(NULL, "#");
    if (strcmp(token, "VIDEO") == 0) {
        //Mensagem vai para a queue de video
        return 1;
    } else {
        //Mensagem vai para a queue de outros
        return 0;
    }
    return -1;
}

int kill_allchildren(int ARM_flag) {
    /* Kills all child processes */
    for (int i=0; i<MAX(settings.AUTH_SERVERS+1, 2); i++) {
        if (child_pids[i]==-1) {
            continue;
        }
        kill(child_pids[i], SIGQUIT);
        if (ARM_flag) { write(AE_unpipes[i][1], "-1#", 4); }    // dummy request to flush AE pipes
        waitpid(child_pids[i], NULL, 0);
    }
    return 0;
}

int handle_request(int id, char *request, char* response) {
    /* Handles request, returns client PID */
    char aux[REQ_SIZE];
    char log_message[BUF_SIZE];
    strcpy(aux, request);

    char *pid = strtok(aux, "#");
    char *arg1 = strtok(NULL, "#");
    char *arg2 = strtok(NULL, "\0");
    int ans;
    //printf("\tpid: %s\n\targ1: %s\n\targ2: %s\n", pid, arg1, arg2);
    if (pid==NULL) {
        return -1;
    }
    //printf("REQUEST: %s\n", request);
    if (arg1!=NULL) {
        if (arg2!=NULL) {
            /* Two args, must be mobile data request */
            ans = check_plafond(atoi(pid), atoi(arg2));
            if (ans==0) {
                // plafond is enough
                response[0] = '\0';
                //sprintf(log_message, "AUTHORIZATION ENGINE %d ACCEPTING CLIENT %d REQUEST %s %s", id, atoi(pid), arg1, arg2);
                //append_logfile(log_message);
            } else if (ans==1) {
                // plafond is not enough
                //delete_client(atoi(pid));     // monitor engine now does this
                response[0] = '\0';
                //sprintf(response, "DISCONNECT");
                //sprintf(log_message, "AUTHORIZATION ENGINE %d DISCONNECTING CLIENT %d", id, atoi(pid));
                //append_logfile(log_message);
            } else {
                // client does not exist
                sprintf(response, "REJECT");
                sprintf(log_message, "AUTHORIZATION ENGINE %d CLIENT %d NOT FOUND", id, atoi(pid));
                append_logfile(log_message);
            }


        } else {
            /* one arg, must be either attempt to connect to system by mobile, or backend request */
            if (arg1[0]>='0' && arg1[0]<='9') {
                // mobile data request
                if (create_client(atoi(pid), atoi(arg1))==0) {
                    // client created
                    sprintf(response, "ACCEPT");
                    sprintf(log_message, "AUTHORIZATION ENGINE %d ACCEPTING NEW CLIENT %d WITH PLAFOND %d.", id, atoi(pid), atoi(arg1));
                    append_logfile(log_message);
                    //printf("CREATING CLIENT\n");
                } else {
                    // client already exists / could not create client
                    sprintf(response, "REJECT");
                    sprintf(log_message, "AUTHORIZATION ENGINE %d REJECTING NEW CLIENT %d", id, atoi(pid));
                    append_logfile(log_message);
                    //printf("REJECTING\n");
                }

            } else {
                // backoffice order
                if ( strcmp(arg1, "reset")==0 ) {
                    // reset
                    append_logfile("AUTHORIZATION_ENGINE RESETING STATS");
                    sprintf(response, "Data has been reset.");
                    reset_stats();
                } else if ( strcmp(arg1, "data_stats")==0 ) {
                    // data_stats
                    int request_count, data_count;
                    append_logfile("AUTHORIZATION_ENGINE FETCHING STATS");
                    fetch_stats(&request_count, &data_count);
                    sprintf(response, "Total requests: %d\tTotal data: %d", request_count, data_count);
                } else {
                    // unknown order
                    sprintf(response, "NON-VALID REQUEST \"%s\".", arg1);
                }
            }

        }
    }

    return atoi(pid);
}

int check_plafond(int pid, int data) {
    /* Check if plafond is enough */
    sem_wait(user_sem);
    int before_plafond = user_array[0].plafond_left;
    for (int i=0; i<settings.MOBILE_USERS; i++) {
        if (user_array[i].id==pid) {
            user_array[i].request_count++;
            if (user_array[i].plafond_left >= data) {
                user_array[i].plafond_left -= data;
                user_array[i].data_count += data;
                if ( before_plafond > 0.1*user_array[i].init_plafond && user_array[i].plafond_left <= 0.1*user_array[i].init_plafond ) {
                    // if plafond is 90% spent, signal to user
                    user_array[i].flag = 1;
                    //pthread_mutex_lock(&monitor_stuff->ME_lock);
                    //pthread_cond_signal(&monitor_stuff->ME_cond);
                    //pthread_mutex_unlock(&monitor_stuff->ME_lock);
                    sem_post(ME_sem);
                    //printf("90%% spent\n\n");
                } else if ( before_plafond > 0.2*user_array[i].init_plafond && user_array[i].plafond_left <= 0.2*user_array[i].init_plafond ) {
                    // if plafond is 80% spent, signal to user
                    user_array[i].flag = 1;
                    //pthread_mutex_lock(&monitor_stuff->ME_lock);
                    //pthread_cond_signal(&monitor_stuff->ME_cond);
                    //pthread_mutex_unlock(&monitor_stuff->ME_lock);
                    sem_post(ME_sem);
                    //printf("80%% spent\n\n");
                }
                sem_post(user_sem);
                return 0;
            } else {
                // plafond is not enough
                user_array[i].data_count += user_array[i].plafond_left;
                user_array[i].plafond_left = 0;
                user_array[i].flag = 1;
                //pthread_mutex_lock(&monitor_stuff->ME_lock);
                //pthread_cond_signal(&monitor_stuff->ME_cond);
                //pthread_mutex_unlock(&monitor_stuff->ME_lock);
                sem_post(ME_sem);
                //printf("100%% spent\n\n");
                sem_post(user_sem);
                return 1;
            }
        }
    }
    sem_post(user_sem);
    return -1;
}

int create_client(int pid, int plafond) {
    /* Create new client */
    sem_wait(user_sem);
    for (int i=0; i<settings.MOBILE_USERS; i++) {
        if (user_array[i].id==-1) {                 // free user space, use it
            user_array[i].id = pid;
            user_array[i].plafond_left = plafond;
            user_array[i].init_plafond = plafond;
            user_array[i].flag = 0;
            sem_post(user_sem);
            return 0;
        } else if (user_array[i].id==pid) {
            sem_post(user_sem);
            return 1;                       // client already exists
        }
    }
    sem_post(user_sem);
    return 1;
}

int delete_client(int pid, int perm_flag) {
    /* Delete client */
    if (!perm_flag) {
        sem_wait(user_sem);
    }
    for (int i=0; i<settings.MOBILE_USERS; i++) {
        if (user_array[i].id==pid) {
            user_array[i].id = -1;
            user_array[i].plafond_left = -1;
            if (!perm_flag) {
                sem_post(user_sem);
            }
            return 0;
        }
    }
    if (!perm_flag) {
        sem_post(user_sem);
    }
    return 1;
}

int reset_stats() {
    /* Reset all stats */
    sem_wait(user_sem);
    for (int i=0; i<settings.MOBILE_USERS; i++) {
        user_array[i].request_count = 0;
        user_array[i].data_count = 0;
    }
    sem_post(user_sem);
    return 0;
}

int fetch_stats(int *request_count, int *data_count) {
    /* Fetch all stats */
    *request_count = 0;
    *data_count = 0;
    sem_wait(user_sem);
    for (int i=0; i<settings.MOBILE_USERS; i++) {
        *request_count += user_array[i].request_count;
        *data_count += user_array[i].data_count;
    }
    sem_post(user_sem);
    return 0;
}