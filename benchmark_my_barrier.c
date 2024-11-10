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

typedef struct{
    int count;
    int thread_count;
    int generation;
    pthread_mutex_t count_mutex;
    pthread_cond_t count_thread_cv;

}my_barrier_t;

my_barrier_t b;

void my_barrier_init(my_barrier_t *b, int count){
    b->count = count;
    b->thread_count = 0;
    b->generation = 0;

    pthread_mutex_init(&(b->count_mutex),NULL);
    pthread_cond_init(&(b->count_thread_cv),NULL);
}

void my_barrier_destroy(my_barrier_t *b){
    pthread_mutex_destroy(&(b->count_mutex));
    pthread_cond_destroy(&(b->count_thread_cv));
}
void my_barrier_wait(my_barrier_t *b){

    
    pthread_mutex_lock(&(b->count_mutex));
    b->thread_count++;
    int current_generation = b->generation;//imi salvez generatia curenta, !! intr - regiune critica
    while (b->thread_count < b->count && current_generation == b->generation) {
        pthread_cond_wait(&(b->count_thread_cv), &(b->count_mutex));
    }
    if(b->thread_count == b->count){ 
        int n = pthread_cond_broadcast(&(b->count_thread_cv));
        if(n != 0){
            perror("ceva s a intamplat\n");
            exit(-1);
        }
        b->thread_count = 0;
        b->generation ++;
    }
    pthread_mutex_unlock(&(b->count_mutex));
}

void *do_work(void *id){
    int thread_id = *(int*)id;
    int sum = 0;
    printf("thread-ul %d is going to do some dummy work \n",thread_id);
    for(int i = 0;i<REPEATS;i++){
        sum = sum + i*thread_id;
    }
    printf("thread-ul %d a calculat suma %d \n",thread_id,sum);
    printf("thread %d is waiting at the barrier \n",thread_id);
    my_barrier_wait(&b);
    printf("\nthread %d has passed the barrier \n",thread_id);

    return NULL;
}
int main(){

    pthread_t threads[NUMTHREADS];
    int ids[NUMTHREADS];

    my_barrier_init(&b,NUMTHREADS);

    for(int i = 0;i<NUMTHREADS;i++){
        ids[i] = i;
        pthread_create(&threads[i],NULL,do_work,(void *)&ids[i]);
    }
    for(int i = 0;i<NUMTHREADS;i++){
        pthread_join(threads[i],NULL);
    }

    my_barrier_destroy(&b);
    return 0;
}