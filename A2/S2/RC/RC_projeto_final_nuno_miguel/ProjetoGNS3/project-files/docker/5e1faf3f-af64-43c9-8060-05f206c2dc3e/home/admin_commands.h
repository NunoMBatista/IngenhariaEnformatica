#ifndef ADMIN_COMMANDS_H
#define ADMIN_COMMANDS_H

#include <netinet/in.h>

int admin_login(char *username, char *password, int client_socket, struct sockaddr_in client_address, socklen_t client_address_len);
void add_user(char *username, char *password, char *role, int client_socket, struct sockaddr_in client_address, socklen_t client_address_len);
void remove_user(char *username, int client_socket, struct sockaddr_in client_address, socklen_t client_address_len);
void list_users(int client_socket, struct sockaddr_in client_address, socklen_t client_address_len);

#endif