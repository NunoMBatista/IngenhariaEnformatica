//include text file and strings
#include <stdio.h>
#include <string.h>

int searchName(char *name){
    FILE *file = fopen("domains.txt", "r");
    
    if(file == NULL){
        printf("Error opening file\n");
        return 1;
    }

    char line[100];
    while(fgets(line, 100, file) != NULL){
        if(strstr(line, name) != NULL){
            strtok(line, " ");
            char *ip = strtok(NULL, " ");
            ip[strlen(ip) - 1] = '\0'; //Remove the '\n' character
            printf("IP: %s\n", ip);
        }
    }
    return 0;
}

int main(int argc, char *argv[]){
    if(argc != 2){
        exit(1); 
    }
    searchName(argv[1]);
    
    return 0;
}