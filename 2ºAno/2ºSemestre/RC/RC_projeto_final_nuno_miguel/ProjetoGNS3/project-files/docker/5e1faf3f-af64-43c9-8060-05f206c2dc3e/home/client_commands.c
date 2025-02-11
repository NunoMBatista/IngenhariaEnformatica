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
#include <netinet/in.h>

#include "global.h"

char *next_available_multicast_ip() {
    // Lock semaphore to access the last assigned multicast address
    sem_wait(classes_sem);
    unsigned int last_assigned_multicast_address = classes_shm->classes_count + BASE_MULTICAST_ADDRESS;
    sem_post(classes_sem);

    // Check if we've run out of multicast IP addresses
    if (last_assigned_multicast_address > MAX_MULTICAST_ADDRESS){
        return NULL;
    }

    // Convert the IP address from an integer to a string
    unsigned int ip = last_assigned_multicast_address++;
    char *ip_str = malloc(16); // Enough space for "239.255.255.255\0"
    sprintf(ip_str, "%u.%u.%u.%u", ip >> 24, (ip >> 16) & 0xFF, (ip >> 8) & 0xFF, ip & 0xFF);

    return ip_str;
}

// LOGIN <username> <password>
User *client_login(char *username, char *password, int client_socket){
    #ifdef DEBUG
    printf("DEBUG# Logging in user %s with password %s...\n", username, password);
    #endif


    char response[BUFLEN];
    User *user = (User*)malloc(sizeof(User));
    user->role[0] = '\0';

    //sprintf(response, "Received request to login as client with username %s and password %s\n", username, password);
    //write(client_socket, response, strlen(response));
    
    for(int i = 0; i < registered_users_count; i++){
        if(strcmp(username, registered_users[i].username) == 0 && strcmp(password, registered_users[i].password) == 0){
            strcpy(user->username, registered_users[i].username);
            strcpy(user->password, registered_users[i].password);
            strcpy(user->role, registered_users[i].role);
            break;
        }
    }
    
    if(user->role[0] == '\0'){
        sprintf(response, "\033[33mREJECTED\n\n\033[0m");
        write(client_socket, response, strlen(response) + 1);
    }
    else if(strcmp(user->role, "administrador") == 0){
        // Only students and professors can login using the TCP client
        sprintf(response, "<This user is an administrator>\nUse an UDP client to access the admin commands\n");
        write(client_socket, response, strlen(response) + 1);

        // Return an empty string to indicate that a role has not been assigned
        User *empty = (User*)malloc(sizeof(User));
        empty->role[0] = '\0';
        return empty;
    }

    // Return student role
    else if(strcmp(user->role, "aluno") == 0){
        sprintf(response, "OK\nLOGGED IN AS STUDENT\n");
        write(client_socket, response, strlen(response) + 1);
    }

    // Return professor role
    else if(strcmp(user->role, "professor") == 0){
        sprintf(response, "OK\nLOGGED IN AS PROFESSOR\n");
        write(client_socket, response, strlen(response) + 1);
    }

    return user;
}
 
// LIST_CLASSES
void list_classes(int client_socket){
    char response[BUFLEN] = "CLASS";
    // sprintf(response, "Received request to list classes\n");
    // write(client_socket, response, strlen(response) + 1);

    #ifdef DEBUG
    printf("DEBUG# Listing every class...\n");
    printf("DEBUG# Accessing shared memory, lock semaphore...\n");
    #endif

    sem_wait(classes_sem);

    if(classes_shm->classes_count == 0){
        sprintf(response, "No classes available\n\n");
        sem_post(classes_sem);
        write(client_socket, response, strlen(response) + 1);
        return;
    }

    for(int i = 0; i < classes_shm->classes_count; i++){
        ClassInfo current_class = classes_shm->classes[i];
        char append[MAX_CLASS_NAME_LEN + 40];
        if(i == 0){
            sprintf(append, " {%s}", current_class.name);
            strcat(response, append);
            continue;
        }
        sprintf(append, ", {%s}", current_class.name);
        strcat(response, append);
    }

    strcat(response, "\n\n");
    sem_post(classes_sem);
    write(client_socket, response, strlen(response) + 1);
    
    #ifdef DEBUG
    printf("DEBUG# Classes listed\n");
    #endif
}

// ------------------------- Student Only -------------------------

// LIST_SUBSCRIBED
void list_subscribed(int client_socket){
    char response[BUFLEN] = "CLASS";
    // sprintf(response, "Received request to list subscribed classes\n");
    // write(client_socket, response, strlen(response) + 1);

    #ifdef DEBUG
    printf("DEBUG# Listing subscribed classes...\n");
    #endif

    if(client_subscribed_classes_count == 0){
        sprintf(response, "No classes subscribed\n\n");
        write(client_socket, response, strlen(response) + 1);
        return;
    }

    char append[MAX_CLASS_NAME_LEN + 40];
    for(int i = 0; i < client_subscribed_classes_count; i++){
        if(i == 0){
            sprintf(append, " {%s/%s}", client_subscribed_classes[i].name, client_subscribed_classes[i].multicast_address);
            strcat(response, append);
            continue;
        }

        sprintf(append, ", {%s/%s}", client_subscribed_classes[i].name, client_subscribed_classes[i].multicast_address);
        printf("DEBUG# Appending %s\n", append);
        strcat(response, append);
    }
    strcat(response, "\n\n");
    write(client_socket, response, strlen(response) + 1);
    #ifdef DEBUG
    printf("DEBUG# Classes listed\n");
    #endif
}

// SUBSCRIBE_CLASS <class_name>
void subscribe_class(char *class_name, int client_socket, User *user_info){
    char response[BUFLEN];
    // sprintf(response, "Received request to subscribe to class %s\n", class_name);
    // write(client_socket, response, strlen(response) + 1);
    
    sem_wait(classes_sem);
    #ifdef DEBUG
    printf("DEBUG# Subscribing to class %s...\n", class_name);
    printf("DEBUG# Accessing shared memory, lock semaphore...\n");
    #endif

    // Check if the class exists
    #ifdef DEBUG
    printf("DEBUG# Checking if class exists...\n");
    #endif
    for(int i = 0; i < classes_shm->classes_count; i++){
        if(strcmp(class_name, classes_shm->classes[i].name) == 0){
            // Check if the student is already subscribed
            #ifdef DEBUG
            printf("DEBUG# Checking if student is already subscribed...\n");
            #endif
            for(int j = 0; j < classes_shm->classes[i].enrolled; j++){
                if(strcmp(user_info->username, classes_shm->classes[i].subscribed_users[j].username) == 0){
                    sprintf(response, "REJECTED, student is already subscribed to class %s\n\n", class_name);
                    sem_post(classes_sem);
                    write(client_socket, response, strlen(response) + 1);
                    return;
                }
            }

            // Check if the class is full
            #ifdef DEBUG
            printf("DEBUG# Checking if class is full...\n");
            #endif
            if(classes_shm->classes[i].enrolled >= classes_shm->classes[i].capacity){
                sprintf(response, "REJECTED, class %s is full\n\n", class_name);
                sem_post(classes_sem);
                write(client_socket, response, strlen(response) + 1);
                return;
            }

            // Subscribe student to class
            #ifdef DEBUG
            printf("DEBUG# Subscribing student to class...\n");
            #endif
            int new_user_index = classes_shm->classes[i].enrolled;
            classes_shm->classes[i].subscribed_users[new_user_index] = *user_info;
            classes_shm->classes[i].enrolled++;

            // Add class to student's subscribed classes
            client_subscribed_classes[client_subscribed_classes_count] = classes_shm->classes[i];
            client_subscribed_classes_count++;
            
            // Accepted message
            sem_post(classes_sem);
            sprintf(response, "ACCEPTED <%s>\n", classes_shm->classes[i].multicast_address);
            write(client_socket, response, strlen(response) + 1);
            return;
        }
    }

    #ifdef DEBUG
    printf("DEBUG# Class %s does not exist\n", class_name);
    #endif
    sem_post(classes_sem);
    sprintf(response, "REJECTED, class %s does not exist\n\n", class_name);
    write(client_socket, response, strlen(response) + 1);
    return;
}

// ------------------------ Professor Only -------------------------

// CREATE_CLASS <class_name> <capacity>
void create_class(char *class_name, int capacity, int client_socket){
    char response[BUFLEN];

    #ifdef DEBUG
    printf("DEBUG# Creating class %s with capacity %d...\n", class_name, capacity);
    #endif
    
    // Get the next available multicast address
    char *multicast_address = next_available_multicast_ip();
    
    // Lock semaphore
    #ifdef DEBUG
    printf("DEBUG# Accessing shared memory, lock semaphore...\n");
    #endif
    sem_wait(classes_sem);

    // Check if the class already exists
    #ifdef DEBUG
    printf("DEBUG# Checking if class already exists...\n");
    #endif
    for(int i = 0; i < classes_shm->classes_count; i++){
        if(strcmp(class_name, classes_shm->classes[i].name) == 0){
            sprintf(response, "REJECTED, class %s already exists\n", class_name);
            sem_post(classes_sem);
            write(client_socket, response, strlen(response) + 1);
            return;
        }
    }

    // Check if the class limit has been reached
    #ifdef DEBUG
    printf("DEBUG# Checking if class limit has been reached...\n");
    #endif   
    if(classes_shm->classes_count >= MAX_CLASSES){
        sprintf(response, "REJECTED, maximum number of classes reached\n\n");
        sem_post(classes_sem);
        write(client_socket, response, strlen(response) + 1);
        return;
    }

    // Setup multicast group
    #ifdef DEBUG
    printf("DEBUG# Start setting up multicast group...\n");
    #endif
    if(multicast_address == NULL){
        sprintf(response, "REJECTED, maximum number of classes reached\n\n");
        sem_post(classes_sem);
        write(client_socket, response, strlen(response) + 1);
        return;
    }

    // Create socket
    #ifdef DEBUG
    printf("DEBUG# Creating socket...\n");
    #endif
    int multicast_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if(multicast_socket < 0){
        perror("<Socket creation failed>\n");
        sem_post(classes_sem);
        exit(1);
    }

    // Set multicast_address_struct to the multicast address and port
    #ifdef DEBUG
    printf("DEBUG# Setting multicast address...\n");
    #endif

    unsigned long int address = inet_addr(multicast_address);

    struct sockaddr_in multicast_address_struct;
    memset(&multicast_address_struct, 0, sizeof(multicast_address_struct));
    multicast_address_struct.sin_family = AF_INET; // IPv4
    multicast_address_struct.sin_addr.s_addr = address; // Multicast address
    multicast_address_struct.sin_port = htons(FIRST_MULTICAST_PORT + address % 1000); // Port
    
    int ttl = 5;
    if(setsockopt(multicast_socket, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl)) < 0){
        printf("<Set TTL failed>\n");
        close(multicast_socket);
        sem_post(classes_sem);
        exit(1);
    }

    // Add class to shared memory
    #ifdef DEBUG
    printf("DEBUG# Adding class to shared memory...\n");
    #endif
    ClassInfo new_class; 
    int class_id = classes_shm->classes_count;
    strcpy(new_class.name, class_name);
    new_class.capacity = capacity;
    new_class.enrolled = 0; 
    strcpy(new_class.multicast_address, multicast_address);
    classes_shm->classes[class_id] = new_class;
    classes_shm->classes[class_id].udp_socket = multicast_socket;
    classes_shm->classes[class_id].udp_address = multicast_address_struct;
    classes_shm->classes_count++;

    // Release semaphore
    #ifdef DEBUG
    printf("DEBUG# Added class to shared memory, release semaphore...\n");
    #endif
    sem_post(classes_sem);

    sprintf(response, "OK <%s>\n\n", multicast_address);
    
    write(client_socket, response, strlen(response) + 1);
}

// SEND <class_name> <message>
void send_message(char *class_name, char *message, int client_socket){
    char response[BUFLEN];

    #ifdef DEBUG
    printf("DEBUG# Sending message to class %s...\n", class_name);
    printf("DEBUG# Accessing shared memory, lock semaphore...\n");
    #endif
    sem_wait(classes_sem);

    // Check if the class exists
    #ifdef DEBUG
    printf("DEBUG# Checking if class exists...\n");
    #endif
    for(int i = 0; i < classes_shm->classes_count; i++){
        ClassInfo current_class = classes_shm->classes[i];
        if(strcmp(current_class.name, class_name) == 0){
            if(sendto(current_class.udp_socket, message, strlen(message) + 1, 0, 
                (struct sockaddr*)&current_class.udp_address, sizeof(current_class.udp_address)) < 0){
                    
                sprintf(response, "REJECTED, failed to send message to class %s\n\n", class_name);
                sem_post(classes_sem);
                write(client_socket, response, strlen(response) + 1);
                return;                    
            }
            sprintf(response, "OK, Message sent to class %s\n\n", class_name);
            sem_post(classes_sem);
            write(client_socket, response, strlen(response) + 1);
            return;
        }
    }

    sprintf(response, "REJECTED, class %s does not exist\n\n", class_name);
    sem_post(classes_sem);
    write(client_socket, response, strlen(response) + 1);
    return;
}