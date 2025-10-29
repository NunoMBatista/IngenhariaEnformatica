#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <signal.h>
#include <pthread.h>
#include <netinet/in.h>

#include "global.h"

#define LOGIN "\t   __             _       \n\t  / /  ___   __ _(_)_ __  \n\t / /  / _ \\ / _` | | '_ \\ \n\t/ /__| (_) | (_| | | | | |\n\t\\____/\\___/ \\__, |_|_| |_|\n\t            |___/         "
#define STUDENT "\t __ _             _            _   \n\t/ _\\ |_ _   _  __| | ___ _ __ | |_ \n\t\\ \\| __| | | |/ _` |/ _ \\ '_ \\| __|\n\t_\\ \\ |_| |_| | (_| |  __/ | | | |_ \n\t\\__/\\__|\\__,_|\\__,_|\\___|_| |_|\\__|\n\t\n"
#define PROFESSOR "\t   ___            __                          \n\t  / _ \\_ __ ___  / _| ___  ___ ___  ___  _ __ \n\t / /_)/ '__/ _ \\| |_ / _ \\/ __/ __|/ _ \\| '__|\n\t/ ___/| | | (_) |  _|  __/\\__ \\__ \\ (_) | |   \n\t\\/    |_|  \\___/|_|  \\___||___/___/\\___/|_|   \n\t\n"

int join_multicast_group(char *multicast_address);
void *receive_multicast_messages(void *multicast_address);
void display_message_in_box(char *message);
char* repeat_char(char c, int length);
void handle_sigint(int sig);

// Socket file descriptor
int current_subscribed_classes = 0;
int client_socket;
int multicast_exit = 0;
// Flag to check if the user is logged in
int logged_in = 0; // 0 if not logged in, 1 if logged in as student, 2 if logged in as professor

char multicast_ips[MAX_SUBSCRIBED_CLASSES][16];
//pthread_t multicast_threads[MAX_SUBSCRIBED_CLASSES];
pthread_t class_threads[MAX_SUBSCRIBED_CLASSES];

struct ip_mreq multicast_groups[MAX_SUBSCRIBED_CLASSES];
int multicast_sockets[MAX_SUBSCRIBED_CLASSES];

int main(int argc, char *argv[]){
    // Clear the screen
    printf("\033[H\033[J");

    signal(SIGINT, handle_sigint);

    // Server's ip address
    char endServer[100]; 
    // Server's address structure containing IP and port
    struct sockaddr_in server_address;
    // Host entry structure containing server's name and IP address
    struct hostent *hostPtr;

    // Check if the number of arguments is correct
    if(argc != 3){
        printf("<Invalid syntax>\n[Correct Usage: ./class_client <server_address> <PORTO_TURMAS>]\n");
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
    if((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        printf("<Socket creation failed>\n");
        return 1;
    }

    // Connect to the server whose address is in server_address
    if(connect(client_socket, (struct sockaddr*) &server_address, sizeof(server_address)) < 0){
        printf("<Connection failed>\n");
        return 1;
    }

    char message_sent[BUFLEN]; // Buffer to store the message sent to the server
    char message_received[BUFLEN]; // Buffer to store the message received from the server

    int bytes_received; // Number of bytes received from the server
   
    char console_string[BUFLEN]; // String always displayed on the console
    strcpy(console_string, LOGIN);
    strcat(console_string, "\n\n\n\n> ");

    while(1){
        // Clear the message_received and message_sent buffers
        memset(message_received, 0, BUFLEN);
        memset(message_sent, 0, BUFLEN);

        // Read the message from the server, BUFLEN - 1 to leave space for the null character
        bytes_received = read(client_socket, message_received, BUFLEN - 1);

        // Check if the user has logged in and change the console string accordingly
        if(logged_in == 0){
            if(strcmp(message_received, "OK\nLOGGED IN AS STUDENT\n") == 0){
                logged_in = 1;
                strcpy(console_string, STUDENT);
                strcat(console_string, "\n\n\n(student) $ ");
            }
            else if(strcmp(message_received, "OK\nLOGGED IN AS PROFESSOR\n") == 0){
                logged_in = 2;
                strcpy(console_string, PROFESSOR);
                strcat(console_string, "\n\n\n(professor) $ ");
            }
        }

        if(bytes_received == 0){
            printf("The server has shut down\n");
            break;
        }
        message_received[bytes_received - 1] = '\0';

        printf("\033[1;36m%s\033[0m", message_received);

        // Check if the message is a multicast message
        char *token = strtok(message_received, " ");
        if(strcmp(token, "ACCEPTED") == 0){
            // Get the multicast address
            token = strtok(NULL, "<");
            token[strlen(token) - 2] = '\0'; // -2 to account the newline and the '<'
            // Join the multicast group
            int multicast_socket = join_multicast_group(token);
            printf("\n\033[1;33m       -> Joined multicast group %s <-\033[0m", token);

            // Create a thread to receive multicast messages
            pthread_create(&class_threads[current_subscribed_classes - 1], NULL, receive_multicast_messages, (void*) &multicast_socket);
        }

        printf("\033[1;32m");
        // Get user input and send it to the server

        printf("\n%s", console_string);        
        fgets(message_sent, BUFLEN - 1, stdin);

        // Clear the screen
        printf("\033[0m \033[H\033[J");

        message_sent[strlen(message_sent) - 1] = '\0';
        // + 1 to include the null character
        write(client_socket, message_sent, strlen(message_sent) + 1);
    }

    close(client_socket);
    return 0; 
}

int join_multicast_group(char *multicast_address){
    // Create a socket for the multicast group
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0){
        printf("<Socket creation failed>\n");
        exit(1);
    }


    // Let the socket reuse the address
    int reuse = 1;
    if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0){
        perror("<setsockopt failed>\n");
        close(sockfd);
        exit(1);
    }

    unsigned int multicast_address_int = inet_addr(multicast_address);
    // Set the multicast_address_struct to the multicast address and port
    struct sockaddr_in multicast_address_struct;

    memset(&multicast_address_struct, 0, sizeof(multicast_address_struct));
    multicast_address_struct.sin_family = AF_INET; // IPv4
    multicast_address_struct.sin_addr.s_addr = htonl(INADDR_ANY); // Multicast address
    multicast_address_struct.sin_port = htons(FIRST_MULTICAST_PORT + multicast_address_int % 1000); // Port
    
    // Bind the socket to the multicast address
    if(bind(sockfd, (struct sockaddr*)&multicast_address_struct, sizeof(multicast_address_struct)) < 0){
        perror("<Bind failed>\n");
        close(sockfd);
        exit(1);
    }

    // Set the multicast group to join
    struct ip_mreq multicast_group;
    multicast_group.imr_multiaddr.s_addr = inet_addr(multicast_address);
    multicast_group.imr_interface.s_addr = htonl(INADDR_ANY);

    // Join the multicast group
    if(setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &multicast_group, sizeof(multicast_group)) < 0){
        printf("<Join failed>\n");
        close(sockfd);
        exit(1);
    } 

    multicast_groups[current_subscribed_classes] = multicast_group;
    multicast_sockets[current_subscribed_classes] = sockfd;
;
    strcpy(multicast_ips[current_subscribed_classes], multicast_address);
    current_subscribed_classes++;

    return sockfd;
}

void *receive_multicast_messages(void *multicast_address){
    int sockfd = *(int*) multicast_address;
    char message[BUFLEN];
    struct sockaddr_in sender_address;
    socklen_t sender_address_length = sizeof(sender_address);

    int nbytes;
    while(!multicast_exit){
        memset(message, 0, BUFLEN);
        if((nbytes = recvfrom(sockfd, message, BUFLEN - 1, 0, (struct sockaddr*)&sender_address, &sender_address_length)) < 0){
            perror("Failed to receive multicast group message\n");
            close(sockfd);
            exit(1);
        }
        if(nbytes == 0){
            printf("The server has shut down\n");
            break;
        }

        // Clear the screen
        printf("\033[36m \033[H\033[J");
        printf("\n\n!!! MESSAGE RECEIVED FROM MULTICAST GROUP !!!\n\n");
        display_message_in_box(message); 

        printf("\n[ENTER TO CONTINUE]\n");

        if(logged_in == 1){
            printf("(student) $ ");
        }
        else if(logged_in == 2){
            printf("(professor) $ ");
        }
    }
    
    return NULL;
}

void display_message_in_box(char *message){
    char *line = strtok(message, "\n");
    int max_len = 0;

    // Find the length of the longest line
    while (line) {
        int len = strlen(line);
        if (len > max_len) {
            max_len = len;
        }
        line = strtok(NULL, "\n");
    }

    printf("\033[1;36m"); // Set color to cyan
    printf("+%s+\n", repeat_char('-', max_len + 2)); // Top border

    // Reset strtok
    line = strtok(message, "\n");
    while (line) {
        printf("| %-*s |\n", max_len, line); // Message line with padding
        line = strtok(NULL, "\n");
    }

    printf("+%s+\n", repeat_char('-', max_len + 2)); // Bottom border
    printf("\033[0m"); // Reset color to default
}

char* repeat_char(char c, int length){
    char* str = malloc(length + 1);
    memset(str, c, length);
    str[length] = '\0';
    return str;
}

void handle_sigint(int sig){
    if(sig == SIGINT){
        // Clear the screen
        printf("\033[H\033[J\033[1;36m");
        multicast_exit = 1;// Closes multicast socket and drops multicast groups on the next work cycle
        for(int i = 0; i < current_subscribed_classes; i++){
            pthread_cancel(class_threads[i]);
            pthread_join(class_threads[i], NULL);
        
            if(setsockopt(multicast_sockets[i], IPPROTO_IP, IP_DROP_MEMBERSHIP, &multicast_groups[i], sizeof(multicast_groups[i])) < 0){
                perror("Failed to drop multicast group\n");
            }
            else{
                printf("\n       -> Left multicast group %s <-\n", multicast_ips[i]);
            }

            if(close(multicast_sockets[i]) < 0){
                perror("Failed to close multicast socket\n");
            }
        }

        printf("\n\t\033[31m       SHUTTING DOWN CLIENT\n\033[0m");
        if(close(client_socket) < 0){
            perror("Failed to close client socket\n");
        }
        exit(0);
    }
}