/******************************************************************************
* FILE: shop.c
* DESCRIPTION:
*   A shop has several moneyiers and one foreman, each represented by a thread. 
*   The threads are created by the main thread.
*   - Some threads represent the shop employees that sell items for 10 eur
*     and put the money in a common safe.
*   - One thread represents the foreman that watches the value of the money. When it 
*     reaches a predefined limit (CASH_LIMIT), he removes an amount of CASH_LIMIT
*     money from the safe, for security reasons.
*   - The program ends after NSALES
******************************************************************************/
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS 8   // Total number of threads (>1)
#define NSALES 30       // Max number of sales made by all employees
#define CASH_LIMIT 50   // Limit of the 'count' variable before it is reset 

int sales = 0;
int	money = 0;

/* initialize money_mutex and money_amount_cv condition variable */
pthread_mutex_t sales_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t money_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t money_amount_cv;
// ...... insert code here .......

void *employee(void *t) {
    int my_id = *((int *)t);

    printf("Starting employee(): thread %d\n", my_id);

    while (1) {

        /* If sales < NSALES make a sell and put money in the safe
        else exits thread */

        sleep(rand()%10);

        pthread_mutex_lock(&sales_mutex);
        if (!(sales<NSALES)) {
            pthread_mutex_unlock(&sales_mutex);
            break;
        }

        pthread_mutex_lock(&money_mutex);
        money += 10;
        sales++;
        pthread_cond_signal(&money_amount_cv);
        printf("Employee: sell number %d! Just sent signal. safe=%d\n", sales, money);
        pthread_mutex_unlock(&sales_mutex);
        pthread_mutex_unlock(&money_mutex);

        // ...... insert code here .......

    }

    pthread_exit(NULL);
}


void *foreman(void *t) {
    int my_id = *((int *)t);

    printf("Starting foreman(): thread %d\n", my_id);

    pthread_mutex_t aux;
    while (1) {

        /* wait if money<CASH_LIMIT and sales<NSALES
        when sales >= NSALES exit thread */
        
        sleep(1);

        pthread_cond_wait(&money_amount_cv, &aux);

        pthread_mutex_lock(&money_mutex);
        if (money<CASH_LIMIT) {
            printf("Foreman does not remove money. Amount = %d\n", money);
            pthread_mutex_unlock(&money_mutex);
            continue;
        }
        money -= 50;
        printf("Foreman removed money! New amount = %d\n", money);
        pthread_mutex_unlock(&money_mutex);


        pthread_mutex_lock(&sales_mutex);
        if (sales>=NSALES) {
            pthread_mutex_unlock(&sales_mutex);
            break;
        }
        pthread_mutex_unlock(&sales_mutex);

        // ...... insert code here .......

    }

    pthread_exit(NULL);
}

int main() {
    int i = 0; 
    int id[NUM_THREADS];
    pthread_t threads[NUM_THREADS];

    pthread_cond_init(&money_amount_cv, NULL);

    /* Create threads */
    id[i] = i;
    pthread_create(&threads[0], NULL, foreman, &id[i]);
    for (i=1; i<NUM_THREADS; i++) {
        id[i] = i;
        pthread_create(&threads[i], NULL, employee, &id[i]);
    }
    // ...... insert code here .......

    /* Wait for all threads to complete */
    for (int i=0; i<NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    // ...... insert code here .......

    printf ("Main(): Waited and joined with %d threads. Final money in safe = %d. Done!\n", 
            NUM_THREADS, money);

    /* Clean up and exit */
    pthread_cond_destroy(&money_amount_cv);
    // ...... insert code here .......

}
