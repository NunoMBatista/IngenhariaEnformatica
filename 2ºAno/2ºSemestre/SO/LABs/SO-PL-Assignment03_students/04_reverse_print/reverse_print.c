#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

#define N 5

void sleep_implementation(){
    if(fork() != 0){
        sleep(N);
        printf("I am %d, the original\n", getpid());
        return;
    }

    for(int i = N-1; i > 0; --i){
        if(fork() == 0){
            continue;
        }
        else{
            sleep(i);
            printf("I am %d and my father is %d\n", getpid(), getppid());
            exit(1);
        }
    }
    exit(1);
}

void wait_implementation(){
    if(fork() != 0){
        wait(NULL);
        printf("I am %d, the original\n", getpid());
        return;
    }
    for(int i = 1; i < N; i++){
        if(fork() != 0){ // Father creates a child and waits for it
            wait(NULL);
            printf("I am %d and my father is %d\n", getpid(), getppid());
            exit(1);
        }
        else{
            //printf("I am %d and my father is %d\n", getpid(), getppid());
            continue;
        }
    }
    exit(1);
}

int main(void){
    printf("Wait implementation:\n");
    wait_implementation();

    printf("Sleep implementation:\n");
    sleep_implementation();

    return 0;
}