#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

#define N 10

int main(void){
    for(int i = 0; i < N; i++){
        if(fork() == 0){
            printf("Process #%d\n", getpid() - getppid());
            break;
        }
        printf("%d: %d\n", getpid(), i);
    }

    return 0;
}



/*
a)

0
1
2
-> 0
    0
    1
    2
    -> 1
        1
        2
        -> 2
            2
    -> 2
        2
-> 1
    1
    2
    -> 2
        2
-> 2
    2
    

*/