#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(){
    int n = 5;
    pid_t child_id, parent_id = getpid();

    for(int i = 0; i <= n; i++){
        child_id = fork();
        if(child_id == 0){
            continue;
        }
        else{
            sleep(n-i);
            if(i == 0){
                printf("\n---\nI am [%d], the original\n", getpid());
                break;
            }
            printf("\n---\nI am [%d] and my father is [%d]\n---\n", getpid(), getppid());
            break;
        }
    }
    return 0;
}