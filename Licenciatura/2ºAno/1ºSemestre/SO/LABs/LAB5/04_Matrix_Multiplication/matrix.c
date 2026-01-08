// Compile as: gcc matrix.c -lpthread -D_REENTRANT -Wall -o matrix

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define M 3 //first matrix lines
#define K 2 //first matrix columns; second matrix lines
#define N 3 //second matrix columns

struct v {
    int row;
    int column;
};

typedef struct v coordenate;
void *worker(void* coord);
void *show_matrix();	// prints matrices on screen

pthread_t my_thread[(M*N)+1];
int A[M][K] = { {1,4}, {2,5}, {3,6} };
int B[K][N] = { {8,7,6}, {5,4,3} };
int C[M][N];

// creates mutex
pthread_mutex_t rows[M];
pthread_mutex_t cols[K];




void print_matrix(){ 
    // prints matrices on screen when having exclusive access

    // lock all mutex
    for (int i=0; i<M; i++) {
        pthread_mutex_lock(&rows[i]);
        //printf("[MUTEX] Locking row %d\n", i);
    }
    for (int i=0; i<N; i++) {
        pthread_mutex_lock(&cols[i]);
        //printf("[MUTEX] Locking column %d\n", i);
    }


    for (int i=0; i<M; i++) {
        for (int ii=0; ii<K; ii++) {
            printf("%4d", A[i][ii]);
        }
        printf("\n");
    }
    printf("X\n");
    for (int i=0; i<K; i++) {
        for (int ii=0; ii<N; ii++) {
            printf("%4d", B[i][ii]);
        }
        printf("\n");
    }
    printf("=\n");
    for (int i=0; i<N; i++) {
        for (int ii=0; ii<M; ii++) {
            printf("%4d", C[i][ii]);
        }
        printf("\n");
    }

    // unlock all mutexs
    for (int i=0; i<M; i++) {
        pthread_mutex_unlock(&rows[i]);
        //printf("[MUTEX] Unlocking row %d\n", i);
    }
    for (int i=0; i<N; i++) {
        pthread_mutex_unlock(&cols[i]);
        //printf("[MUTEX] Unlocking column %d\n", i);
    }
}

int main(void) {
    // creates threads and sends each the coordinates of the element to calculate
    pthread_create(&my_thread[0], NULL, show_matrix, NULL);
    coordenate *coord;
    for (int m=0; m<M; m++) {
        for (int n=0; n<N; n++) {
            coord = malloc(sizeof(struct v));
            coord->row = m;
            coord->column = n;
            pthread_create(&my_thread[((m*N)+n)+1], NULL, worker, coord);
            printf("[THREAD] Worker %d for m->%d n->%d created.\n", ((m*N)+n), coord->row, coord->column);
        }
    }
    

    // waits for threads to finish
    pthread_join(my_thread[0], NULL);
    for (int m=0; m<M; m++) {
        for (int n=0; n<N; n++) {
            pthread_join(my_thread[((m*N)+n)+1], NULL);
            printf("[THREAD] Worker %d joined.\n", ((m*N)+n));
        }
    }
    

    // prints final results of the calculation and leaves
    printf("-----------------------------------------\n");
    printf("FINAL RESULT:\n");
    print_matrix();

}


void *worker(void* coord) {
    // calculates each matrix element in the coordinate received
    coordenate v = *((coordenate*)coord);
    int mult_res = 0;
    free(coord);

    int sleep_time = rand()%10;
    printf("Thread sleeping for %d seconds. wroo mimimim\n", sleep_time);
    sleep(sleep_time);
    printf("Thread awoke.\n");

    pthread_mutex_lock(&rows[v.row]);
    pthread_mutex_lock(&cols[v.column]);
    //printf("[MUTEX] Locking row %d\n", v.row);
    //printf("[MUTEX] Locking column %d\n", v.column);

    for (int i=0; i<K; i++) {
        mult_res += A[v.row][i]*B[i][v.column];
    }
    C[v.row][v.column] = mult_res;

    pthread_mutex_unlock(&rows[v.row]);
    pthread_mutex_unlock(&cols[v.column]);
    //printf("[MUTEX] Unlocking row %d\n", v.row);
    //printf("[MUTEX] Unlocking column %d\n", v.column);
    return NULL;
}

void *show_matrix() {
    // prints the status of the matrix in each second, along a period of 10 seconds

    for (int i=0; i<=10; i++) {
        printf("\n-----> T+ %ds <-----\n", i);
        print_matrix();
        print_matrix("\n");
        sleep(1);
    }

    return NULL;
}
