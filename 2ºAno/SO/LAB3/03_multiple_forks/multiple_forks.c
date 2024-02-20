#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <math.h>

int main(){
    int N = 8;
    
    //b) - Method 1
    //N = log(N)/log(2);
    
    for(int i = 0; i < N; i++){
        pid_t child_id = fork();
        printf("%d: %d\n", getpid(), i);

        // b) - Method 2
        // if(child_id == 0){
        //     break;
        // }
    }

    return 0;
}
/*
    a) 8 processes are created (including the father process), it always creates 2^N processes

    b) In order to create exactly N processes we need to loop log2(N) times
*/