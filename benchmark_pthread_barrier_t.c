#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<unistd.h>
#include<stdlib.h>
/*
Create a benchmark program where you create NUMTHREADS threads. All threads will participate in a barrier. 
Each thread executes a loop repeating it REPEATS times where it is doing some dummy work in do_work function and
then waits at the barrier. The do_work function can create a dummy workload by adding  N float numbers.

for (i = 0; i < REPEATS; i++) {

do_work(thread_id);

barrier_wait(&barrier);
}
*/

#define NUMTHREADS 5
#define REPEATS 10

pthread_barrier_t b;

void *do_work(void *id){
    int thread_id = *(int*)id;
    int sum = 0;
    printf("thread-ul %d is going to do some dummy work \n",thread_id);
    for(int i = 0;i<REPEATS;i++){
        sum = sum + i;
    }
    printf("thread-ul %d a calculat suma %d \n",thread_id,sum);
    printf("thread %d is waiting at the barrier \n",thread_id);
    pthread_barrier_wait(&b);
    printf("\nthread %d has passed the barrier \n",thread_id);

    return NULL;
}
int main(){

    pthread_t threads[NUMTHREADS];
    int ids[NUMTHREADS];

    pthread_barrier_init(&b, NULL, NUMTHREADS);

    for(int i = 0;i<NUMTHREADS;i++){
        ids[i] = i;
        pthread_create(&threads[i],NULL,do_work,(void *)&ids[i]);
    }
    for(int i = 0;i<NUMTHREADS;i++){
        pthread_join(threads[i],NULL);
    }

    pthread_barrier_destroy(&b);

    
    return 0;
}