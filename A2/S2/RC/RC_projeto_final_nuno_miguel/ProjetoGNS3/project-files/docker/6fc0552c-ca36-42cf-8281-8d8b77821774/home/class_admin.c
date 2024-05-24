#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <signal.h>
#include "global.h"

#define ADMIN "\t   _       _           _       \n\t  /_\\   __| |_ __ ___ (_)_ __  \n\t //_\\\\ / _` | '_ ` _ \\| | '_ \\ \n\t/  _  \\ (_| | | | | | | | | | |\n\t\\_/ \\_/\\__,_|_| |_| |_|_|_| |_|\n\t                               "

// Socket file descriptor
int client_socket;

void handle_sigint(int sig);

int main(int argc, char *argv[]){
    // Clear the screen
    printf("\033[H\033[J\n\n\n");

    signal(SIGINT, handle_sigint);

    // Server's ip address
    char endServer[100]; 
    // Server's address structure containing IP and port
    struct sockaddr_in server_address;
    // Host entry structure containing server's name and IP address
    struct hostent *hostPtr;

    // Check if the number of arguments is correct
    if(argc != 3){
        printf("<Invalid syntax>\n[Correct Usage: ./class_admin <server_address> <PORTO_CONFIG>]\n");
        return 1;
    }

    // Check port validity
    int PORTO_TURMAS = atoi(argv[2]);
    // Ports below 1024 are reserved for system services and ports above 4999 are invalid
    if(PORTO_TURMAS < 1024 || PORTO_TURMAS >= FIRST_MULTICAST_PORT){
        printf("<Invalid port>\n[Port must be integers between 1024 and 4999]\n");
        return 1;
    }

    strcpy(endServer, argv[1]); // Copy server's ip address to endServer
    if((hostPtr = gethostbyname(endServer)) == 0){
        printf("<Address not found>\n[Address %s not found]\n", endServer);
        return 1;
    }

    // Clear server_address structure
    bzero((void*) &server_address, sizeof(server_address)); 
    // Set the server_address family to IPv4
    server_address.sin_family = AF_INET;
    // Set the server's ip address
    server_address.sin_addr.s_addr = ((struct in_addr *)(hostPtr->h_addr_list[0]))->s_addr;
    // Set the server's port to the port passed as argument
    server_address.sin_port = htons((short) PORTO_TURMAS);
    
    // Create a socket and store its file descriptor in client_socket
    if((client_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
        printf("<Socket creation failed>\n");
        return 1;
    }

    char message_sent[BUFLEN]; // Buffer to store the message sent to the server
    char message_received[BUFLEN]; // Buffer to store the message received from the server

    int bytes_received; // Number of bytes received from the server
    socklen_t server_address_length = sizeof(server_address); // Length of the server's address
    
    char console_string[BUFLEN]; // String always displayed on the console
    strcpy(console_string, ADMIN);
    strcat(console_string, "\n\n\n\n(admin) # ");

    while(1){
        // Clear the message_received and message_sent buffers
        memset(message_received, 0, BUFLEN);
        memset(message_sent, 0, BUFLEN);

        printf("\033[1;33m");
        // Get user input and send it to the server
        printf("%s", console_string);

        fgets(message_sent, BUFLEN - 1, stdin);
        message_sent[strlen(message_sent) - 1] = '\0';

        // Clear the screen
        printf("\033[0m\033[H\033[J");

        // + 1 to include the null character
        sendto(client_socket, message_sent, strlen(message_sent) + 1, 0, (struct sockaddr*) &server_address, server_address_length);

        // Read the message from the server, BUFLEN - 1 to leave space for the null character
        bytes_received = recvfrom(client_socket, message_received, BUFLEN - 1, 0, (struct sockaddr*) &server_address, &server_address_length);
        if(bytes_received == 0){
            printf("The server has shut down\n");
            break;
        }
        message_received[bytes_received - 1] = '\0';

        printf("\033[1;36m%s\033[0m\n", message_received);
    }

    close(client_socket);
    return 0; 
}

void handle_sigint(int sig){
    if(sig == SIGINT){
        printf("\nSHUTTING DOWN ADMIN\n");
        close(client_socket);
        exit(0);
    }
}