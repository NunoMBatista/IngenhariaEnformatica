#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

int lineCounter(FILE *file){
    int counter = 0;
    char ch;
    while((ch = fgetc(file)) != EOF){
        if(ch == '\n'){
            counter++;
        }
    }
    rewind(file);
    return counter;
}

int compareStr(char* str1, char* str2){ 
    int out = 1;
    
    for(int i = 0; i < (int)sizeof(str1); i++){
        if(str1[i] != str2[i]){
            out = 0;
        }
    }
    return out;
}

char* decryptLine(char *line, int nKeys, char decryptPairs[nKeys][2][100]){
    char *token;
    token = strtok(line, " ");
    while(token != NULL){
        int encrypted = 0;
        for(int i = 0; i < nKeys; i++){
            if(compareStr(decryptPairs[i][1], token)){
                encrypted = 1;
                printf("%s ", decryptPairs[i][0]);
            }
        }
        if(!encrypted){
            printf("%s ", token);
        }
        token = strtok(NULL, " ");
    }
    return line;
}

int main(int argc, char *argv[]){
    if(argc != 3){
        printf("Input file name and decryption keys");
        return 1;
    }

    char line[300];

    char *fileName = argv[1]; 
    FILE *filePtr = fopen(fileName, "r");

    // Get decripytion pairs
    char *decryptName = argv[2];
    FILE *decryptPtr = fopen(decryptName, "r");
    int nKeys = lineCounter(decryptPtr);
    
    char decryptPairs[nKeys][2][100];
    int curIdx = 0;

    char *token;
    while(fgets(line, sizeof(line), decryptPtr) != NULL){
        token = strtok(line, " ");
        strcpy(decryptPairs[curIdx][0], token);
        token = strtok(NULL, " ");
        token[strlen(token)-1] = '\0';
        printf("%d\n", (int)strlen(token));
        printf("%s\n", token);
        strcpy(decryptPairs[curIdx][1], token);
        curIdx++;
    }
    // End get decryption pairs

    for(int i = 0; i < nKeys; i++){
        printf("%s and %s", decryptPairs[i][0], decryptPairs[i][1]);
    }

    pid_t pid, father_pid = getpid();
    int c = 0;
    while(fgets(line, sizeof(line), filePtr) != NULL){
        c++;
        pid = fork();
        if(pid == 0){
            printf("[%d]", getpid());
            decryptLine(line, nKeys, decryptPairs);
        }
        else{
            wait(&pid);
            break;
        }
    }
    wait(NULL);

    if(pid == 0){
        printf("%d lines Translated\n", c);
        exit(0);
    }
    
    
    return 0;
}