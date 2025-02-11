#ifndef GLOBAL_H
#define GLOBAL_H

#include <semaphore.h>
#include <netinet/in.h>

#define BUFLEN 1024
#define MAX_PASSWORD_LEN 50
#define MAX_USERNAME_LEN 50
#define MAX_ROLE_LEN 50
#define MAX_CLASS_NAME_LEN 50
#define MAX_REGISTERED_USERS 100
#define MAX_SUBSCRIBED_CLASSES 20
#define MAX_CLASS_CAPACITY 100
#define FIRST_MULTICAST_PORT 5000
#define MAX_CLASSES 100
#define BASE_MULTICAST_ADDRESS 0xEF000001 // 239.0.0.1
#define MAX_MULTICAST_ADDRESS 0xEFFFFFFF // 239.255.255.255
//extern unsigned int last_assigned_multicast_port;
//extern unsigned int last_assigned_multicast_address;

#define DEBUG // Comment this line to disable debug messages
#define SEMAPHORE_NAME "classes_sem"

// Defines a single user
typedef struct user{
    char username[BUFLEN];
    char password[BUFLEN];
    char role[BUFLEN];
} User;

// Defines a single class
typedef struct class_info{
    char name[MAX_CLASS_NAME_LEN];
    int capacity;
    int enrolled;
    char multicast_address[16];
    int udp_socket;
    struct sockaddr_in udp_address;
    User subscribed_users[MAX_CLASS_CAPACITY];
} ClassInfo; 

// Shared memory structure for classes
typedef struct classes{
    int classes_count;
    ClassInfo classes[MAX_CLASSES];
} Classes;

// Shared memory structure for classes
extern Classes *classes_shm;
extern int classes_shm_id;

// Semaphore for classes
extern sem_t *classes_sem;

// Count of registered users
extern int registered_users_count;
// Array of registered users
extern User registered_users[MAX_REGISTERED_USERS];

// Array of classes subscribed by the current processes' client
extern ClassInfo client_subscribed_classes[MAX_SUBSCRIBED_CLASSES];
// Count of classes subscribed by the current processes' client
extern int client_subscribed_classes_count;

#endif