/* 
 * Versão de 1ªMeta da disciplina Sistemas Operativos
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
int create_sharedmem();

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

/* Utils */
int check_requesttype(char *request);
int kill_allchildren(int ARM_flag);
int handle_request(int id, char *request, char* response);

// client stuff
int check_plafond(int pid, int data);
int create_client(int pid, int plafond);
int delete_client(int pid);



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
    int init_plafond;           // initial plafond
    int plafond_left;           // plafond left ( >= 0 disconnect )
} User_data;



// General stuff
sem_t *log_sem;
struct Settings settings;
int pid;
int system_manager_pid;
int running = 1;

// Shared memory stuff
sem_t *user_sem;
int shmid;
User_data *user_array;

// Store child pids of the given process
int child_count;
int *child_pids;
int (*AE_unpipes)[2];     // only for ARM

// Message queue
int message_queue_id;

// process name stuff
int max_processname_size;
char *process_name;


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

    /* Create shared memory */
    if ( create_sharedmem()!=0 ) {
        system_panic(-1);
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
    sem_close(log_sem);         // }
    sem_unlink("log_sem");      // } unlink and close log_sem
    sem_close(user_sem);            // }
    sem_unlink("user_sem");         // } unlink and close user_sem
    shmdt(user_array);                  // }
    shmctl(shmid, IPC_RMID, 0);         // } free shared memory

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

    append_logfile("AUTHORIZATION_REQUEST_MANAGER CLOSING");

    exit(0);
}

void close_authorization_engine() {
    /* Used to close AUTHORIZATION_ENGINE */
    append_logfile("AUTHORIZATION_ENGINE CLOSING");

    running = 0;    // stop the while loop in AE, so that AE can exit only when it finishes what it's doing
}



int create_sharedmem() {
    /* Create sharedmemory (user_array) */
    shmid = shmget(IPC_PRIVATE, sizeof(User_data) * settings.MOBILE_USERS, IPC_CREAT | 0777);
    if (shmid == -1) {
        fprintf(stderr, "[ERROR] Could not create shared memory\n");
        return 1;
    }
    user_array = (User_data*) shmat(shmid, NULL, 0);
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



int append_logfile(char *log_info) {
    sem_wait(log_sem);
    FILE *log = fopen(LOG_FILE, "a");
    if (log==NULL) {
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
    fprintf(log,    "%s %s\n", time_buffer, log_info);      // } Write log info to log.txt
    fprintf(stdout, "%s %s\n", time_buffer, log_info);      // } and to terminal

    fclose(log);
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

    AE_unpipes = (int(*)[2]) malloc(sizeof(int[2]) * settings.AUTH_SERVERS+1);

    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, close_authorization_request_manager);

    pthread_t reciever, sender;
    append_logfile("PROCESS AUTHORIZATION_REQUEST_MANAGER CREATED");

    // Create queues
    queue q[2];
    pthread_cond_t written = PTHREAD_COND_INITIALIZER;
    pthread_mutex_t written_lock = PTHREAD_MUTEX_INITIALIZER;
    create_queue(&q[0], settings.QUEUE_POS, BUF_SIZE, &written, &written_lock);
    create_queue(&q[1], settings.QUEUE_POS, BUF_SIZE, &written, &written_lock);

    
    for (int i=0; i<settings.AUTH_SERVERS; i++) {
        if (pipe(AE_unpipes[i])<0) {
            append_logfile("[ERROR] COULD NOT CREATE UNNAMED PIPE FOR AE\n");
            system_panic();
        }
        parallel_AuthorizationEngine(i+1);
    }

    pthread_create(&reciever, NULL, receiver_ARM, q);
    pthread_create(&sender, NULL, sender_ARM, q);
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
        child_pids[child_count] = pid;
        child_count++;
        return 0;
    }
    memset(process_name, '\0', max_processname_size);   // }
    strcpy(process_name, "AUTH_ENGINE");                // } change process name to AE

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
    while (running) {       // "running" is to make the AE not exit during an operation
        // read from unnamed pipe AE_unpipes[n-1][0]
        read_n = read(AE_unpipes[n-1][0], request, BUF_SIZE);
        if (read_n==0) {
            // if read returns 0, pipe is closed, exit
            break;
        }
        //printf("[AE %d] %s\n", id, request);
        cli_pid = handle_request(id, request, response);
        if (cli_pid==-1) {
            break;
        }
        if (response[0]!='\0') {
            // if response is not NULL, send response to client
            //printf("[AE %d] RESPONDING TO %d-\"%s\"\n", id, cli_pid, response);
            msg.mtype = cli_pid;
            strcpy(msg.mtext, response);
            msgsnd(message_queue_id, &msg, sizeof(msg), 0);
        }
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

    append_logfile("PROCESS MONITOR_ENGINE CREATED");
    while (1) {}        // TODO[META1] do MONITOR_ENGINE
    exit(0);
}



void *receiver_ARM( void *arg ) {
    /* Reciever (AUTHORIZATION_REQUEST_MANAGER) */
    char inbuffer[BUF_SIZE];
    int mobile_pipe_fd, backend_pipe_fd;

    append_logfile("THREAD RECEIVER CREATED");
    
    queue *video_queue = (queue *)arg;
    queue *others_queue = (queue *)arg+1;


    if ( (mkfifo(BACKEND_PIPE, O_CREAT|O_EXCL|0600)<0) && (errno!=EEXIST) ) {
        // Creates the BACKEND named pipe if it doesn't exist yet
        perror("Cannot create pipe: ");
        system_panic();
    } else if ((backend_pipe_fd = open(BACKEND_PIPE, O_RDWR)) < 0) {
        // Opens BACKEND named pipe for reading
        perror("Cannot open pipe for reading: ");
        system_panic();
    } else {
        append_logfile("BACKEND PIPE CREATED");
    }

    if ( (mkfifo(MOBILE_PIPE, O_CREAT|O_EXCL|0600)<0) && (errno!=EEXIST) ) {
        // Creates the BACKEND named pipe if it doesn't exist yet
        perror("Cannot create pipe: ");
        system_panic();
    } else if ((mobile_pipe_fd = open(MOBILE_PIPE, O_RDWR)) < 0) {
        // Opens BACKEND named pipe for reading
        perror("Cannot open pipe for reading: ");
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
                if (read(backend_pipe_fd, &inbuffer, sizeof(inbuffer))!=EOF) {
                    //printf("[RECEIVED] BACKEND -> %s\n", inbuffer);
                }
            }
            if (FD_ISSET(mobile_pipe_fd, &readfds)) {
                // If is written to mobile_pipe_fd
                if (read(mobile_pipe_fd, &inbuffer, sizeof(inbuffer))!=EOF) {
                    //printf("[RECEIVED] MOBILE -> %s\n", inbuffer);
                }
            }
            
            /* check request type, and write to right queue */
            if (check_requesttype(inbuffer) ) {
                write_queue(video_queue, inbuffer);
                //printf("[WRITE-VIDEO] %s\n", inbuffer);
            } else {
                write_queue(others_queue, inbuffer);
                //printf("[WRITE-OTHERS] %s\n", inbuffer);
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

    char request[BUF_SIZE];

    int next_AE = 0;
    while (1) {
        // Wait for a signal to read from the queues
        pthread_mutex_lock(video_queue->cond_lock);
        pthread_cond_wait(video_queue->cond, video_queue->cond_lock);
        pthread_mutex_unlock(video_queue->cond_lock);

        while (1) {
            // read until BOTH queues are empty
            if (count_queue(video_queue)>0) {
                // Priority to video queue
                read_queue(video_queue, request);
                //printf("[READ-VIDEO] %s\n", request);
            } else if (count_queue(others_queue)>0) {
                // If video queue is empty, others queue must have smth
                read_queue(others_queue, request);
                //printf("[READ-OTHERS] %s\n", request);
            } else {
                // If both queues are empty, wait for a signal
                break;
            }

            // Write to first "next" AE that is alive and free
            while (1) {
                if (next_AE>=settings.AUTH_SERVERS+1) {
                    // wrap around child_pids
                    next_AE = 0;
                }
                if (child_pids[next_AE]!=-1) {
                    // if AE is alive
                    if (write(AE_unpipes[next_AE][1], request, strlen(request)+1) == -1) {
                        // if write fails, AE is dead, kill it
                        append_logfile("[FAILURE] UNPIPE TO AE BROKEN, KILLING AE");
                        kill(child_pids[next_AE], SIGQUIT);
                        child_pids[next_AE] = -1;
                    }

                    next_AE++;
                    break;
                }
                next_AE++;
            }

        }
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
    for (int i=0; i<settings.AUTH_SERVERS+1; i++) {
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
    char aux[BUF_SIZE];
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
    if (arg1!=NULL) {
        if (arg2!=NULL) {
            /* Two args, must be mobile data request */
            ans = check_plafond(atoi(pid), atoi(arg2));
            if (ans==0) {
                // plafond is enough
                response[0] = '\0';
            } else if (ans==1) {
                // plafond is not enough
                delete_client(atoi(pid));
                sprintf(response, "DISCONNECT");
                sprintf(log_message, "AUTHORIZATION ENGINE %d DISCONNECTING CLIENT %d", id, atoi(pid));
                append_logfile(log_message);
            } else {
                // client does not exist
                response[0] = '\0';
                sprintf(log_message, "AUTHORIZATION ENGINE %d CLIENT %d NOT FOUND", id, atoi(pid));
                append_logfile(log_message);
            }

        } else {
            /* one arg, must be either attempt to connect to system by mobile, or backend request */
            if (create_client(atoi(pid), atoi(arg1))==0) {
                // client created
                sprintf(response, "ACCEPT");
                sprintf(log_message, "AUTHORIZATION ENGINE %d ACCEPTING NEW CLIENT %d", id, atoi(pid));
                append_logfile(log_message);
                //printf("CREATING CLIENT\n");
            } else {
                // client already exists / could not create client
                sprintf(response, "REJECT");
                sprintf(log_message, "AUTHORIZATION ENGINE %d REJECTING NEW CLIENT %d", id, atoi(pid));
                append_logfile(log_message);
                //printf("REJECTING\n");
            }
        }
    }

    return atoi(pid);
}

int check_plafond(int pid, int data) {
    /* Check if plafond is enough */
    sem_wait(user_sem);
    for (int i=0; i<settings.MOBILE_USERS; i++) {
        if (user_array[i].id==pid) {
            if (user_array[i].plafond_left >= data) {
                user_array[i].plafond_left -= data;
                sem_post(user_sem);
                return 0;
            } else {
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

int delete_client(int pid) {
    /* Delete client */
    sem_wait(user_sem);
    for (int i=0; i<settings.MOBILE_USERS; i++) {
        if (user_array[i].id==pid) {
            user_array[i].id = -1;
            user_array[i].plafond_left = -1;
            sem_post(user_sem);
            return 0;
        }
    }
    sem_post(user_sem);
    return 1;
}

