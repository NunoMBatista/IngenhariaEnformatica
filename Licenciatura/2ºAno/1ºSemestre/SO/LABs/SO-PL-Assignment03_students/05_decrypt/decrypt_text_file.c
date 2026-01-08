#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINE_LENGTH 1024
#define MAX_DECRYPT_KEYS 7
#define MAX_DECRYPT_KEY_LENGTH 50

int nKeys;

void decryptLine(char* line, char decryptPairs[MAX_DECRYPT_KEYS][2][MAX_DECRYPT_KEY_LENGTH]);
void readTextFile(FILE* file, char decryptPairs[MAX_DECRYPT_KEYS][2][MAX_DECRYPT_KEY_LENGTH]);
void printDecrypted(char* word, char decryptPairs[MAX_DECRYPT_KEYS][2][MAX_DECRYPT_KEY_LENGTH]);
int compareStrings(char* str1, char* str2);
char* removeSpacesAndNewlines(char* input);

int main(int argc, char* argv[]){
    if(argc != 2){
        printf("Incorrect number of arguments\n");
        return 1;
    }

    FILE * decryptFile = fopen("decrypt.txt", "r");
    if(decryptFile == NULL){
        printf("Could not open file\n");
        return 1;
    }

    char line[MAX_LINE_LENGTH];
    char* token;
    char decryptPairs[MAX_DECRYPT_KEYS][2][MAX_DECRYPT_KEY_LENGTH];
    int decryptIdx = 0; 
    while(fgets(line, sizeof(line), decryptFile)){
        token = strtok(line, " ");
        strcpy(decryptPairs[decryptIdx][0], token);
        
        printf("%s ", decryptPairs[decryptIdx][0]);

        token = strtok(NULL, " ");
        token = removeSpacesAndNewlines(token);
        
        strcpy(decryptPairs[decryptIdx][1], token);
        printf("%s\n", decryptPairs[decryptIdx++][1]);

        decryptIdx++;
    }
    nKeys = decryptIdx;

    FILE *file = fopen(argv[1], "r");
    if(file == NULL){
        printf("Could not open file\n");
        return 1;
    }

    readTextFile(file, decryptPairs);

    return 0;
}

void readTextFile(FILE* file, char decryptPairs[MAX_DECRYPT_KEYS][2][MAX_DECRYPT_KEY_LENGTH]){
    char line[MAX_LINE_LENGTH];
    pid_t pid;
    int teste; 
    while(teste = fgets(line, sizeof(line), file)){        
        pid = fork();
        if(pid == 0){
            printf("Line decrypted by [%d]:", getpid(), getppid());
         
            decryptLine(line, decryptPairs);   
            fclose(file);

            exit(0);
        }
    }
    while(wait(NULL) > 0);
}

void decryptLine(char* line, char decryptPairs[MAX_DECRYPT_KEYS][2][MAX_DECRYPT_KEY_LENGTH]){
    char* token = strtok(line, " ");
    printDecrypted(token, decryptPairs);

    while((token = strtok(NULL, " "))){
        printDecrypted(token, decryptPairs);
    }
}

void printDecrypted(char* word, char decryptPairs[MAX_DECRYPT_KEYS][2][MAX_DECRYPT_KEY_LENGTH]){
    //word = removeSpacesAndNewlines(word);
    for(int i = 0; i < nKeys; ++i){
        //printf("\n------\n%s = %s\n-------\n", word, decryptPairs[i][1]);

        if(compareStrings(word, decryptPairs[i][1]) == 0){
            printf("%s ", decryptPairs[i][0]);
            return;
        }
    }
    printf("%s ", word);
    return;
}

char* removeSpacesAndNewlines(char* input) {
    char* result = malloc(strlen(input) + 1);
    int i, j = 0;

    for(i = 0; input[i] != '\0'; i++) {
        if(input[i] != ' ' && input[i] != '\n') {
            result[j++] = input[i];
        }
    }

    result[j] = '\0';
    return result;
}

int compareStrings(char* str1, char* str2){
    int i = 0; 
    while(str1[i] != '\0'){
      //  printf("%c\n", str2[i]);

        if((str1[i] != str2[i]) || (str2[i] == '\0')){
            return 1;
        }
        i++;
    }
    return 0;
}