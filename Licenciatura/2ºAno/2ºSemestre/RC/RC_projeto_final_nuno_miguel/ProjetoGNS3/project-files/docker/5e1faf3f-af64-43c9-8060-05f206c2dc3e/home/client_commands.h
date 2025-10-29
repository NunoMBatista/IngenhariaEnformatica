#ifndef CLIENT_COMMANDS_H
#define CLIENT_COMMANDS_H
#include "global.h"

char *next_available_multicast_ip();

User* client_login(char *username, char *password, int client_socket);
void list_classes(int client_socket);

// Student only
void list_subscribed(int client_socket);
void subscribe_class(char *class_name, int client_socket, User *user_info);

// Professor only
void create_class(char *class_name, int capacity, int client_socket);
void send_message(char *class_name, char *message, int client_socket);

#endif