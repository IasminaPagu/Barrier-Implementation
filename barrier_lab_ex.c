
/* File:
 *    barrier.c
 * It illustrates the use of Barriers for threads synchronization
 * Main creates a number of THREAD_COUNT=4 threads
 * Each thread is represented by the function threadFn
 * Each thread executes a Stage 1 having a variable duration(each thread sleeps 
 * a number of seconds equal to its thread id) and then waits at a barrier 
 * until all threads have finished their Stage 1 in order to start Stage 2
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define THREAD_COUNT 4

pthread_barrier_t mybarrier;

void *threadFn(void *id_ptr)
{
    int thread_id = *(int *)id_ptr;

    printf("thread %d: staying for %d seconds in Stage 1.\n", thread_id, thread_id);

    /* simulates work in stage 1*/
    /* each thread has a different duration of Stage 1 - equal with its rank (id)*/
    sleep(thread_id); 

    printf("thread %d: Stage 1 finished ... \n", thread_id);

    pthread_barrier_wait(&mybarrier);

    printf("thread %d: starting Stage 2!\n", thread_id);

    return NULL;
}

int main(int argc, char *argv[]){

    int i;

    pthread_t threads[THREAD_COUNT];
    int ids[THREAD_COUNT];

    pthread_barrier_init(&mybarrier, NULL, THREAD_COUNT);

    for (i = 0; i < THREAD_COUNT; i++)
    {
        ids[i]=i;
        pthread_create(&threads[i], NULL, threadFn, (void *)&ids[i]);
    }


    for (i = 0; i < THREAD_COUNT; i++)
    {
        pthread_join(threads[i], NULL);
    }

    pthread_barrier_destroy(&mybarrier);
    return 0;
}