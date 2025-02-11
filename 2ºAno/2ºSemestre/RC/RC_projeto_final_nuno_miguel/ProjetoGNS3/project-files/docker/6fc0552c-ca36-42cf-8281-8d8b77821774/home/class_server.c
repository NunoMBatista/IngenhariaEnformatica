#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <sys/socket.h> 
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h> 
#include <semaphore.h>

#include "admin_commands.h"
#include "client_commands.h"
#include "global.h"

//unsigned int last_assigned_multicast_port = FIRST_MULTICAST_PORT;
unsigned int last_assigned_multicast_address = BASE_MULTICAST_ADDRESS;

User registered_users[MAX_REGISTERED_USERS];

int registered_users_count = 0; 
int admin_logged_in = 0; // Boolean to check if an admin is logged in
pid_t main_process_id;

Classes *classes_shm;
int classes_shm_id;

sem_t *classes_sem;

ClassInfo client_subscribed_classes[MAX_SUBSCRIBED_CLASSES];
int client_subscribed_classes_count = 0;

int tcp_socket;
int udp_socket;

// void pointer return type is used to run the functions in threads
void* handle_udp(void* udp_port_ptr);
void* handle_tcp(void* tcp_port_ptr);
void process_client(int client_socket);
void handle_sigint(int sig);
void interpret_client_command(char* command, int client_socket, User** user_info);
void interpret_admin_command(char* command, int client_socket, struct sockaddr_in client_address, socklen_t client_address_len);
int read_configuration_file(char* file_name);

/* 
    The main function should create a socket that can receive both TCP and UDP messages.
    Execution syntax: ./class_server <PORTO_TURMAS> <PORTO_CONFIG> <configuration file>
*/
int main(int argc, char *argv[]){
    signal(SIGINT, handle_sigint);
    main_process_id = getpid();

    // Check if the number of arguments is correct
    if(argc != 4){
        printf("<Invalid syntax>\n[Correct Usage: ./class_server <PORTO_TURMAS> <PORTO_CONFIG> <configuration file>]\n");
        return 1;
    }
    
    // Check port validity
    int PORTO_TURMAS = atoi(argv[1]);
    int PORTO_CONFIG = atoi(argv[2]);
    // Ports below 1024 are reserved for system services and ports above 65535 are invalid
    if(PORTO_TURMAS < 1024 || PORTO_TURMAS >= FIRST_MULTICAST_PORT || PORTO_CONFIG < 1024 || PORTO_CONFIG >= FIRST_MULTICAST_PORT){
        printf("<Invalid port>\n[Port must be integers between 1024 and %d]\n", FIRST_MULTICAST_PORT);
        return 1;
    }

    // Check if PORTO_TURMAS and PORTO_CONFIG are different
    if(PORTO_TURMAS == PORTO_CONFIG){
        printf("<Invalid port>\n[PORTO_TURMAS and PORTO_CONFIG must be different]\n");
        return 1;
    }

    // Check if the configuration file exists
    FILE *file = fopen(argv[3], "r");
    if(file == NULL){
        printf("<File not found>\n[File %s not found]\n", argv[3]);
        return 1;
    }
    fclose(file);

    registered_users_count = read_configuration_file(argv[3]);

    // Create shared memory for classes
    classes_shm_id = shmget(IPC_PRIVATE, sizeof(Classes), IPC_CREAT | 0666);
    if(classes_shm_id == -1){
        perror("Error creating shared memory");
        exit(1);
    }
    if((classes_shm = (Classes*) shmat(classes_shm_id, NULL, 0)) == (void*) -1){
        perror("Error attaching shared memory");
        exit(1);
    }

    classes_shm->classes_count = 0; // Initialize the number of classes

    // Initialize the semaphore
    sem_close(classes_sem);
    sem_unlink(SEMAPHORE_NAME);
    classes_sem = sem_open(SEMAPHORE_NAME, O_CREAT, 0666, 1);
    if(classes_sem == SEM_FAILED){
        perror("Error creating semaphore");
        exit(1);
    }


    pthread_t udp_thread, tcp_thread;
    // Start listening for UDP messages
    pthread_create(&udp_thread, NULL, handle_udp, &PORTO_CONFIG);
    
    // Start listening for TCP messages
    pthread_create(&tcp_thread, NULL, handle_tcp, &PORTO_TURMAS);
    
    pthread_join(udp_thread, NULL);
    pthread_join(tcp_thread, NULL);

    return 0; 
}

void* handle_tcp(void* tcp_port_ptr){
    int tcp_port = *((int*) tcp_port_ptr);

    // Create socket for TCP messages
    // AF_INET: IPv4 IP
    // SOCK_STREAM: TCP socket type
    // 0: Use default protocol (TCP)
    tcp_socket = socket(AF_INET, SOCK_STREAM, 0);

    // Check if the socket was created successfully
    if(tcp_socket == -1){
        perror("Error creating TCP socket");
        exit(1);
    }

    // Create a sockaddr_in struct to store the server address and port
    struct sockaddr_in server_address;

    // Set every byte of server_address to 0 to ensure no garbage values are present
    memset(&server_address, 0, sizeof(server_address));

    // Set the server address family to IPv4
    server_address.sin_family = AF_INET;
    // Set the server port's to the port passed as argument
    server_address.sin_port = htons(tcp_port); // htons() converts host's byte order (unspecified) to network's byte order (big-endian)
    // Set the server address to any available address in the system (INADDR_ANY)
    server_address.sin_addr.s_addr = htonl(INADDR_ANY); // htonl() is the same as htons() but for 32-bit values

    // Bind the socket to the server address and port, which are stored in a sockaddr struct
    if(bind(tcp_socket, (struct sockaddr*) &server_address, sizeof(server_address)) == -1){
        perror("Error binding TCP socket");
        close(tcp_socket);
        exit(1);
    }

    // Start listening for incoming connections
    if(listen(tcp_socket, 5) == -1){
        perror("Error listening for incoming connections");
        close(tcp_socket);
        exit(1);
    }

    char buffer[BUFLEN];
    while(1){
        // Clear the buffer before receiving a new message
        memset(buffer, 0, BUFLEN);

        // Accept a new connection
        // These variables must be declared inside the loop in TCP to avoid using the same memory address for different clients
        struct sockaddr_in client_address;
        socklen_t client_address_len = sizeof(client_address);
        // Create a new socket for the client and store the client's address in client_address
        int client_socket = accept(tcp_socket, (struct sockaddr*) &client_address, &client_address_len);
        if(client_socket == -1){
            perror("Error accepting connection");
            break;
        }

        // Fork a new process to handle the client
        pid_t pid = fork();
        if(pid == -1){
            perror("Error forking process");
            break;
        }

        if(pid == 0){
            // Close the copy of the server socket in the child process
            close(tcp_socket);

            // Process a client
            process_client(client_socket);

            // Exit the child process
            exit(0);
        }
        else{
            // Close the copy of the client socket in the parent process
            close(client_socket);
        }
    }
    close(tcp_socket);
    return NULL;
}

void process_client(int client_socket){
    #ifdef DEBUG
    printf("DEBUG# Client connected\n");
    #endif

    write(client_socket, "\033[036m\n    -- Welcome to the class server --\n\033[0m", 50);

    //char user_role[15]; // Store the user's role
    //user_role[0] = '\0';
    User *user_info = (User*) malloc(sizeof(User));
    user_info->role[0] = '\0';

    int bytes_received = 0;
    char buffer[BUFLEN];
    char response[BUFLEN + 1]; // + 1 to account for the "\n" suffix on the client's end

    // Clear the buffer and response before receiving a new message
    memset(buffer, 0, BUFLEN);
    memset(response, 0, BUFLEN);

    // Read until the client disconnects
    while((bytes_received = read(client_socket, buffer, BUFLEN - 1)) > 0){
        buffer[bytes_received - 1] = '\0'; // Add null terminator to the end of the message
        if(bytes_received == -1){
            perror("Error reading message\n");
            break;
        }

        #ifdef DEBUG
        printf("DEBUG# Client message received - %s\n", buffer);
        #endif

        // Interpret the command
        interpret_client_command(buffer, client_socket, &user_info);
        
        // Clear the buffer and response before receiving a new message
        memset(buffer, 0, BUFLEN);
        memset(response, 0, BUFLEN);
    }
    if(bytes_received == 0){
        printf("Client disconnected\n");
    }
    else{
        perror("Error reading message\n");
    }

    // Remove user from subscribed classes
    sem_wait(classes_sem);
    for(int i = 0; i < client_subscribed_classes_count; i++){
        for(int j = 0; j < classes_shm->classes_count; j++){
            if(strcmp(client_subscribed_classes[i].name, classes_shm->classes[j].name) == 0){
                for(int k = 0; k < classes_shm->classes[j].enrolled; k++){
                    if(strcmp(classes_shm->classes[j].subscribed_users[k].username, user_info->username) == 0){
                        for(int l = k; l < classes_shm->classes[j].enrolled - 1; l++){
                            classes_shm->classes[j].subscribed_users[l] = classes_shm->classes[j].subscribed_users[l + 1];
                        }
                        classes_shm->classes[j].enrolled--;
                        break;
                    }
                }
            }   
        }
    }
    
    sem_post(classes_sem);

    // Clear the user_info struct
    user_info->role[0] = '\0';
    free(user_info);
    
    // Close the client socket
    close(client_socket);
}

void* handle_udp(void *udp_port_ptr){
    // Cast the void pointer to an integer
    int udp_port = *((int*) udp_port_ptr);
    int bytes_received = 0;

    // Create socket for UDP messages
    // AF_INET: IPv4 IP
    // SOCK_DGRAM: UDP socket type
    // 0: Use default protocol (UDP)
    udp_socket = socket(AF_INET, SOCK_DGRAM, 0);

    // Check if the socket was created successfully
    if(udp_socket == -1){
        perror("Error creating UDP socket");
        exit(1);
    }

    // Create a sockaddr_in struct to store the server address and port
    struct sockaddr_in server_address;

    // Set every byte of server_address to 0 to ensure no garbage values are present
    memset(&server_address, 0, sizeof(server_address)); 
    // Set the server address family to IPv4
    server_address.sin_family = AF_INET;
    // Set the server port to the port passed as an argument
    server_address.sin_port = htons(udp_port); // htons() converts host's byte order (unspecified) to network's byte order (big-endian)
    // Set the server address to any available address in the system (INADDR_ANY)
    server_address.sin_addr.s_addr = htonl(INADDR_ANY); // htonl() is the same as htons() but for 32-bit values

    // Bind the socket to the server address and port, which are stored in a sockaddr struct
    if(bind(udp_socket, (struct sockaddr*) &server_address, sizeof(server_address)) == -1){
        perror("Error binding UDP socket");
        close(udp_socket);
        exit(1);
    }

    // After setting up and binding the socket, start listening for messages
    char buffer[BUFLEN];
    memset(buffer, 0, BUFLEN);

    // Create a sockaddr_in struct to store the client's address
    struct sockaddr_in client_address;
    socklen_t client_address_len = sizeof(client_address);

    while(1){
        // Clear the buffer before receiving a new message
        memset(buffer, 0, BUFLEN);

        // Receive a message from the client and store the client's address in client_address
        // recvfrom() is a blocking function, meaning the program will wait until a message is received
        // BUFFLEN - 1 to leave space for the null terminator
        bytes_received = recvfrom(udp_socket, buffer, BUFLEN - 1, 0, (struct sockaddr*) &client_address, &client_address_len);
        buffer[bytes_received-1] = '\0'; // Remove the newline character from the message
        if(bytes_received == -1){
            perror("Error receiving message");
            break;
        }

        #ifdef DEBUG
        printf("DEBUG# Admin message from %s:%d - %s\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port), buffer); 
        #endif

        // Interpret the command
        interpret_admin_command(buffer, udp_socket, client_address, client_address_len);
    }

    // Close the socket
    close(udp_socket);
    return NULL;
}

// Double pointer is used to change the value of the user_info pointer in the main function when the user logs in
void interpret_client_command(char* command, int client_socket, User **user_info){
    char* token = strtok(command, " ");
    if(token == NULL){
        write(client_socket, "\033[1;31m<Empty command>\n\n\033[0m", 29);
        return;
    }
    
    if(strcmp("LOGIN", token) == 0){
        // Check if the user is already logged in
        if((*user_info)->role[0] != '\0'){
            write(client_socket, "\033[1;31m<Already logged in>\n\n\033[0m", 33);
            return;
        }

        char *error_message = "\033[1;31m<Invalid command>\nCorrect Usage: LOGIN <username> <password>\n\033[0m";
        // Check if the command has the correct number of arguments
        char* username = strtok(NULL, " ");
        char* password = strtok(NULL, " ");
        if(username == NULL || password == NULL){
            write(client_socket, error_message, strlen(error_message) + 1);
            return;
        }
        // Check if there are no more arguments
        if(strtok(NULL, " ") != NULL){
            write(client_socket, error_message, strlen(error_message) + 1);
            return;
        }
        
        //User* user;

        *user_info = client_login(username, password, client_socket);
        //user_info = user; 
        return;
    }

    // Check if the user is logged in before executing any other command
    else if((*user_info)->role[0] == '\0'){
        write(client_socket, "\033[1;31m<Not logged in>\n\n\033[0m", 29);
        return;
    }

    if(strcmp("LIST_CLASSES", token) == 0){
        char *error_message = "\033[1;31m<Invalid command>\nCorrect Usage: LIST_CLASSES\n\033[0m";
        // Check if the command has the correct number of arguments
        if(strtok(NULL, " ") != NULL){
            write(client_socket, error_message, strlen(error_message) + 1);
            return;
        }
        list_classes(client_socket);
        return;
    }

    if(strcmp("LIST_SUBSCRIBED", token) == 0){
        if(strcmp((*user_info)->role, "aluno") != 0){
            write(client_socket, "\033[1;31m<Unauthorized command>\nYou need to be a student to list subscribed classes\n\033[0m", 87);
            return;
        }
        char *error_message = "\033[1;31m<Invalid command>\nCorrect Usage: LIST_SUBSCRIBED\n\033[0m";
        // Check if the command has the correct number of arguments
        if(strtok(NULL, " ") != NULL){
            write(client_socket, error_message, strlen(error_message) + 1);
            return;
        }
        list_subscribed(client_socket);
        return;
    }

    if(strcmp("SUBSCRIBE_CLASS", token) == 0){
        #ifdef DEBUG
        printf("DEBUG# Received subscribe command\n");
        printf("DEBUG# Checking parameters...\n");
        #endif

        if(strcmp((*user_info)->role, "aluno") != 0){
            write(client_socket, "\033[1;31m<Unauthorized command>\nYou need to be a student to subscribe to a class\n\033[0m", 84);
            return;
        }

        char *error_message = "\033[1;31m<Invalid command>\nCorrect Usage: SUBSCRIBE <class_name>\n\033[0m";
        // Check if the command has the correct number of arguments
        char* class_name = strtok(NULL, " ");
        if(class_name == NULL){
            write(client_socket, error_message, strlen(error_message) + 1);
            return;
        }
        // Check if there are no more arguments
        if(strtok(NULL, " ") != NULL){
            write(client_socket, error_message, strlen(error_message) + 1);
            return;
        }
        subscribe_class(class_name, client_socket, *user_info);
        return;
    }

    if(strcmp("CREATE_CLASS", token) == 0){
        if(strcmp((*user_info)->role, "professor") != 0){
            write(client_socket, "\033[1;31m<Unauthorized command>\nYou need to be a professor to create a class\n\033[0m", 80);
            return;
        }

        #ifdef DEBUG
        printf("DEBUG# Received create class command\n");
        printf("DEBUG# Checking parameters...\n");
        #endif

        char *error_message = "\033[1;31m<Invalid command>\nCorrect Usage: CREATE_CLASS <class_name> <capacity>\n\033[0m";
        // Check if the command has the correct number of arguments
        char* class_name = strtok(NULL, " ");
        char* capacity_str = strtok(NULL, " ");
        if(class_name == NULL || capacity_str == NULL){
            write(client_socket, error_message, strlen(error_message) + 1);
            return;
        }

        // Check parameter length
        if(strlen(class_name) > MAX_CLASS_NAME_LEN || strlen(capacity_str) > MAX_CLASS_CAPACITY){
            write(client_socket, "\033[1;31m<Invalid command>\nClass name must have less than 50 characters and capacity must be an integer\n\033[0m", 107);
            return;
        }

        for(int i = 0; i < (int)strlen(capacity_str); i++){
            if(capacity_str[i] < '0' || capacity_str[i] > '9'){
                write(client_socket, error_message, strlen(error_message) + 1);
                return;
            }
        }

        int capacity = atoi(capacity_str);
        // Check capacity validity
        if(capacity < 1 || capacity > MAX_CLASS_CAPACITY){
            write(client_socket, "\033[1;31m<Invalid command>\nCapacity must be an integer between 1 and 100\n\033[0m", 76);
            return;
        }

        // Check if there are no more arguments
        if(strtok(NULL, " ") != NULL){
            write(client_socket, error_message, strlen(error_message) + 1);
            return;
        }


        #ifdef DEBUG
        printf("DEBUG# Parameters are valid\n");
        printf("DEBUG# Creating class...\n");
        #endif        

        create_class(class_name, capacity, client_socket);
        return;
    }

    if(strcmp("SEND", token) == 0){
        if(strcmp((*user_info)->role, "professor") != 0){
            write(client_socket, "\033[1;31m<Unauthorized command>\nYou need to be a professor to send a message to a class\n\033[0m", 91);
            return;
        }
        char *error_message = "\033[1;31m<Invalid command>\nCorrect Usage: SEND <class_name> <message>\n\033[0m";
        // Check if the command has the correct number of arguments
        char* class_name = strtok(NULL, " ");
        char* message = strtok(NULL, "");
        if(class_name == NULL || message == NULL){
            write(client_socket, error_message, strlen(error_message) + 1);
            return;
        }
        // Check if there are no more arguments
        if(strtok(NULL, " ") != NULL){
            write(client_socket, error_message, strlen(error_message) + 1);
            return;
        }
        send_message(class_name, message, client_socket);
        return;
    }
    
    write(client_socket, "\033[1;31m<Invalid command>\n\n\033[0m", 31);
}

void interpret_admin_command(char* command, int client_socket, struct sockaddr_in client_address, socklen_t client_address_len){
    #ifdef DEBUG
    printf("DEBUG# Received admin command: %s\n", command);
    #endif   

    char *token = strtok(command, " ");
    if(token == NULL){
        sendto(client_socket, "\033[1;31m<Invalid command>\n\n\033[0m", 30, 0, (struct sockaddr*) &client_address, client_address_len);
        return;
    }

    if(strcmp("LOGIN", token) == 0){
        // Check if the admin is already logged in
        if(admin_logged_in == 1){
            sendto(client_socket, "\033[1;31m<Already logged in as admin>\n\n\033[0m", 41, 0, (struct sockaddr*) &client_address, client_address_len);
            return;
        }

        char *error_message = "\033[1;31m<Invalid command>\nCorrect Usage: LOGIN <username> <password>\n";
        // Check if the command has the correct number of arguments
        char* username = strtok(NULL, " ");
        char* password = strtok(NULL, " ");
        if(username == NULL || password == NULL){
            sendto(client_socket, error_message, strlen(error_message), 0, (struct sockaddr*) &client_address, client_address_len);
            return;
        }
        // Check if there are no more arguments
        if(strtok(NULL, " ") != NULL){
            sendto(client_socket, error_message, strlen(error_message), 0, (struct sockaddr*) &client_address, client_address_len);
            return;
        }

        admin_logged_in = admin_login(username, password, client_socket, client_address, client_address_len);
        return;
    }

    if(admin_logged_in == 0){
        sendto(client_socket, "\033[1;31m<Unauthorized command>\nYou need to be logged in as an admin to use this console\n\033[0m", 92, 0, (struct sockaddr*) &client_address, client_address_len);
        return;
    }

    if(strcmp("ADD_USER", token) == 0){
        char *error_message = "\033[1;31m<Invalid command>\nCorrect Usage: ADD_USER <username> <password> <role>\n\n\033[0m";
        // Check if the command has the correct number of arguments
        char* username = strtok(NULL, " ");
        char* password = strtok(NULL, " ");
        char* role = strtok(NULL, " ");


        if(username == NULL || password == NULL || role == NULL){
            sendto(client_socket, error_message, strlen(error_message), 0, (struct sockaddr*) &client_address, client_address_len);
            return;
        }

        // Check parameter length
        if(strlen(username) > MAX_USERNAME_LEN || strlen(password) > MAX_PASSWORD_LEN || strlen(role) > MAX_ROLE_LEN){
            sendto(client_socket, "\033[1;31m<Invalid command>\nUsername, password and role must have less than 50 characters\n\033[0m", 92, 0, (struct sockaddr*) &client_address, client_address_len);
            return;
        }
        
        // Check role validity
        if(strcmp(role, "administrador") != 0 && strcmp(role, "aluno") != 0 && strcmp(role, "professor") != 0){
            sendto(client_socket, "\033[1;31m<Invalid command>\nRole must be 'administrador', 'aluno' or 'professor'\n\033[0m", 83, 0, (struct sockaddr*) &client_address, client_address_len);
            return;
        }

        // Check if there are no more arguments
        if(strtok(NULL, " ") != NULL){
            sendto(client_socket, error_message, strlen(error_message), 0, (struct sockaddr*) &client_address, client_address_len);
            return;
        }
        add_user(username, password, role, client_socket, client_address, client_address_len);
        return;
    }

    if(strcmp("DEL", token) == 0){
        char *error_message = "\033[1;31m<Invalid command>\nCorrect Usage: DEL <username>\n\033[0m";
        // Check if the command has the correct number of arguments
        char* username = strtok(NULL, " ");
        if(username == NULL){
            sendto(client_socket, error_message, strlen(error_message), 0, (struct sockaddr*) &client_address, client_address_len);
            return;
        }
        // Check if there are no more arguments
        if(strtok(NULL, " ") != NULL){
            sendto(client_socket, error_message, strlen(error_message), 0, (struct sockaddr*) &client_address, client_address_len);
            return;
        }
        remove_user(username, client_socket, client_address, client_address_len);
        return;
    }

    if(strcmp("LIST", token) == 0){
        char *error_message = "\033[1;31m<Invalid command>\nCorrect Usage: LIST\n\n\033[0m";
        // Check if the command has the correct number of arguments
        if(strtok(NULL, " ") != NULL){
            sendto(client_socket, error_message, strlen(error_message), 0, (struct sockaddr*) &client_address, client_address_len);
            return;
        }
        
        list_users(client_socket, client_address, client_address_len);
        return;
    }

    if(strcmp("QUIT_SERVER", token) == 0){
        char *error_message = "\033[1;31m<Invalid command>\nCorrect Usage: QUIT_SERVER\n\n\033[0m";
        // Check if the command has the correct number of arguments
        if(strtok(NULL, " ") != NULL){
            sendto(client_socket, error_message, strlen(error_message), 0, (struct sockaddr*) &client_address, client_address_len);
            return;
        }

        sendto(client_socket, "Server shutting down...\n", 25, 0, (struct sockaddr*) &client_address, client_address_len);

        handle_sigint(SIGINT); 
        //shutdown_server(client_socket, client_address, client_address_len);
        return;
    }

    sendto(client_socket, "\033[1;31m<Invalid command>\n\n\033[0m", 31, 0, (struct sockaddr*) &client_address, client_address_len);
}

int read_configuration_file(char* file_name){
    FILE *file = fopen(file_name, "r");
    if(file == NULL){
        perror("Error opening configuration file");
        exit(1);
    }

    char buffer[BUFLEN];
    int current_user = 0;

    // Read the file line by line
    while(fgets(buffer, BUFLEN, file) != NULL){
        char original_buffer[BUFLEN];
        strcpy(original_buffer, buffer);

        char* token = strtok(buffer, ";"); // Split the line by the ';' character
        if(token == NULL || strchr(token, ' ') != NULL){ // Skip lines with spaces
            printf("Line contains spaces in configuration file, line %s\n", original_buffer);
            continue;
        }

        User* new_user = (User*) malloc(sizeof(User));
        strcpy(new_user->username, token);

        token = strtok(NULL, ";");
        if(token == NULL || strchr(token, ' ') != NULL){
            printf("Password contains spaces in configuration file, line %s\n", original_buffer);
            free(new_user); // Skip lines with spaces in password
            continue;
        }
        strcpy(new_user->password, token);

        token = strtok(NULL, ";");
        if(token == NULL || strchr(token, ' ') != NULL){ // Skip lines with spaces in role
            printf("Role contains spaces in configuration file, line %s\n", original_buffer);
            free(new_user);
            continue;
        }

        token[strcspn(token, "\n")] = 0; // Remove the newline character from the end of the string
        // Check if the role is valid
        if(strcmp(token, "administrador") != 0 && strcmp(token, "aluno") != 0 && strcmp(token, "professor") != 0){
            printf("Invalid role in configuration file, line %s\n", original_buffer);
            free(new_user);
            continue;
        }
        strcpy(new_user->role, token);


        // Check if there are no more arguments
        if(strtok(NULL, ";") != NULL){
            printf("Invalid line in configuration file\n");
            exit(1);
        }

        // Check if the maximum number of registered users has been reached
        if(current_user >= MAX_REGISTERED_USERS){
            printf("Maximum number of registered users reached\n");
            break;
        }

        // Check for duplicate usernames
        for(int i = 0; i < current_user; i++){
            if(strcmp(registered_users[i].username, new_user->username) == 0){
                printf("Duplicate username in configuration file\n");
                free(new_user);
                continue;
            }
        }

        // Add the user to the list of registered users
        registered_users[current_user] = *new_user;
        current_user++;
    }
    fclose(file);
    return current_user;
}

void handle_sigint(int sig){
    if(sig == SIGINT){
        close(tcp_socket);
        close(udp_socket);

        // If the process is the main process, print a message before exiting
        if(getpid() == main_process_id){
            printf("\nSHUTTING DOWN SERVER\n");
            
            // Detach and remove the shared memory
            if(shmdt(classes_shm) == -1){
                perror("Error detaching shared memory");
            }
            if(shmctl(classes_shm_id, IPC_RMID, NULL) == -1){
                perror("Error removing shared memory");
            }

            // Close and unlink the semaphore
            if(sem_close(classes_sem) == -1){
                perror("Error closing semaphore");
            }
            if(sem_unlink(SEMAPHORE_NAME) == -1){
                perror("Error unlinking semaphore");
            }
        }

        exit(0);
    }
}
