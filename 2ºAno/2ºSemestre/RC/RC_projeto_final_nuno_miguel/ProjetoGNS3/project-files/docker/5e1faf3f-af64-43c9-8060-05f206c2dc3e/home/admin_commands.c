#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include "global.h"

// LOGIN <username> <password>
int admin_login(char *username, char *password, int client_socket, struct sockaddr_in client_address, socklen_t client_address_len){
    char response[BUFLEN];

    // Check if the user is registered
    for(int i = 0; i < registered_users_count; i++){
        if(strcmp(username, registered_users[i].username) == 0 && strcmp(password, registered_users[i].password) == 0){
            // Send a message to the client indicating that the login was successful
            if(strcmp(registered_users[i].role, "administrador") == 0){
                sprintf(response, "OK\nLOGGED IN AS ADMIN\n");
                sendto(client_socket, response, strlen(response) + 1, 0, (struct sockaddr*) &client_address, client_address_len);
                return 1;
            }

            sprintf(response, "REJECTED, USER %s IS NOT AN ADMIN\n\n", username);
            sendto(client_socket, response, strlen(response) + 1, 0, (struct sockaddr*) &client_address, client_address_len);
            return 0;
        }
    }

    sprintf(response, "REJECTED, USER %s NOT FOUND\n\n", username);
    sendto(client_socket, response, strlen(response) + 1, 0, (struct sockaddr*) &client_address, client_address_len);
    return 0;
}

// ADD_USER <username> <password> <role>
void add_user(char *username, char *password, char *role, int client_socket, struct sockaddr_in client_address, socklen_t client_address_len){
    char response[BUFLEN];
    //sprintf(response, "Received request to add user %s with password %s and role %s\n", username, password, role);
    //sendto(client_socket, response, strlen(response) + 1, 0, (struct sockaddr*) &client_address, client_address_len);

    // Check if the user is already registered
    for(int i = 0; i < registered_users_count; i++){
        if(strcmp(username, registered_users[i].username) == 0){
            sprintf(response, "REJECTED, USER %s ALREADY REGISTERED\n\n", username);
            sendto(client_socket, response, strlen(response) + 1, 0, (struct sockaddr*) &client_address, client_address_len);
            return;
        }
    }

    // Check if the maximum number of registered users has been reached
    if(registered_users_count == MAX_REGISTERED_USERS){
        sprintf(response, "REJECTED, MAXIMUM NUMBER OF REGISTERED USERS REACHED\n\n");
        sendto(client_socket, response, strlen(response) + 1, 0, (struct sockaddr*) &client_address, client_address_len);
        return;
    }

    // Add the user to the list of registered users
    strcpy(registered_users[registered_users_count].username, username);
    strcpy(registered_users[registered_users_count].password, password);
    strcpy(registered_users[registered_users_count].role, role);
    registered_users_count++;

    // Add user to config file
    FILE *config_file = fopen("configuration.conf", "a");
    fprintf(config_file, "\n%s;%s;%s", username, password, role);
    fclose(config_file);
    
    sprintf(response, "USER %s REGISTERED as %s\n\n", username, role);
    sendto(client_socket, response, strlen(response) + 1, 0, (struct sockaddr*) &client_address, client_address_len);
}

// DEL <username>
void remove_user(char *username, int client_socket, struct sockaddr_in client_address, socklen_t client_address_len){
    char response[BUFLEN];
    // sprintf(response, "Received request to remove user %s\n", username);
    // sendto(client_socket, response, strlen(response) + 1, 0, (struct sockaddr*) &client_address, client_address_len);

    // Check if the user is registered
    for(int i = 0; i < registered_users_count; i++){
        if(strcmp(username, registered_users[i].username) == 0){
            // Remove the user from the list of registered users
            // Shift all elements to the left
            for(int j = i; j < registered_users_count - 1; j++){
                strcpy(registered_users[j].username, registered_users[j + 1].username);
                strcpy(registered_users[j].password, registered_users[j + 1].password);
                strcpy(registered_users[j].role, registered_users[j + 1].role);
            }
            registered_users_count--;

            // Remove user from config file
            FILE *config_file = fopen("configuration.conf", "w");
            // Rewrite the file without the removed user
            for(int j = 0; j < registered_users_count; j++){
                if(j == 0)
                    fprintf(config_file, "%s;%s;%s", registered_users[j].username, registered_users[j].password, registered_users[j].role);
                else
                    fprintf(config_file, "\n%s;%s;%s", registered_users[j].username, registered_users[j].password, registered_users[j].role);
            }
            fclose(config_file);

            sprintf(response, "USER %s REMOVED\n\n", username);
            sendto(client_socket, response, strlen(response) + 1, 0, (struct sockaddr*) &client_address, client_address_len);
            return;
        }
    }

    sprintf(response, "REJECTED, USER %s NOT FOUND\n\n", username);
    sendto(client_socket, response, strlen(response) + 1, 0, (struct sockaddr*) &client_address, client_address_len);
}

// LIST
void list_users(int client_socket, struct sockaddr_in client_address, socklen_t client_address_len){
    char response[BUFLEN + (registered_users_count * 1000)];

    // Send the list of registered users to the client
    sprintf(response, "\033[H\033[J\n--REGISTERED USERS--\n\n");
    for(int i = 0; i < registered_users_count; i++){
        sprintf(response + strlen(response), "-> Username: %s\n   Password: %s\n   Role: %s\n\n", registered_users[i].username, registered_users[i].password, registered_users[i].role);
    }

    sendto(client_socket, response, strlen(response) + 1, 0, (struct sockaddr*) &client_address, client_address_len);
}

// // QUIT_SERVER
// void shutdown_server(int client_socket, struct sockaddr_in client_address, socklen_t client_address_len){
//     // char response[BUFLEN];
//     // sprintf(response, "Received request to shutdown server\n");
//     // sendto(client_socket, response, strlen(response) + 1, 0, (struct sockaddr*) &client_address, client_address_len);
// }
