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

#define NUMTHREADS 100
#define REPEATS 30

typedef struct{
    int count;
    int thread_count;
    int generation;
    pthread_mutex_t count_mutex;
    pthread_cond_t count_thread_cv;

}my_barrier_t;

my_barrier_t b1;//bariera declarat folosind implementarea mea
pthread_barrier_t b2;//bariera declarata folosind POSIX implementation

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
//functia care face un dummy workload folosind implementarea mea
void *do_work1(void *id){
    int thread_id = *(int*)id;
    int sum = 0;
    printf("thread-ul %d is going to do some dummy work \n",thread_id);
    for(int i = 0;i<REPEATS;i++){
        sum = sum + i*thread_id;
    }
    printf("thread-ul %d a calculat suma %d \n",thread_id,sum);
    printf("thread %d is waiting at the barrier \n",thread_id);
    my_barrier_wait(&b1);
    printf("\nthread %d has passed the barrier \n",thread_id);

    return NULL;
}
//functia care face dummy workload folosind implementarea POSIX
void *do_work2(void *id){
    int thread_id = *(int*)id;
    int sum = 0;
    printf("thread-ul %d is going to do some dummy work \n",thread_id);
    for(int i = 0;i<REPEATS;i++){
        sum = sum + i*thread_id;
    }
    printf("thread-ul %d a calculat suma %d \n",thread_id,sum);
    printf("thread %d is waiting at the barrier \n",thread_id);
    pthread_barrier_wait(&b2);
    printf("\nthread %d has passed the barrier \n",thread_id);

    return NULL;
}
int main(){

    pthread_t threads1[NUMTHREADS];
    pthread_t threads2[NUMTHREADS];

    int ids1[NUMTHREADS];
    printf("\n\nalgoritmul care foloseste functiile implementate de mine decurge astfel \n\n");


    //aici incep cronometrarea pentru functiile implementate de mine
    struct timespec start1, finish1;
    double elapsed1;

    struct timespec start2, finish2;
    double elapsed2;

    clock_gettime(CLOCK_MONOTONIC, &start1); // measure wall clock time!
    my_barrier_init(&b1,NUMTHREADS);

    for(int i = 0;i<NUMTHREADS;i++){
        ids1[i] = i;
        pthread_create(&threads1[i],NULL,do_work1,(void *)&ids1[i]);
    }
    for(int i = 0;i<NUMTHREADS;i++){
        pthread_join(threads1[i],NULL);
    }
    my_barrier_destroy(&b1);
    
    clock_gettime(CLOCK_MONOTONIC, &finish1);
    elapsed1 = (finish1.tv_sec - start1.tv_sec);
    elapsed1 += (finish1.tv_nsec - start1.tv_nsec) / 1000000000.0;


    int ids2[NUMTHREADS];
    printf("\n\nalgoritmul care foloseste functiile din biblioteca POSIX decurge astfel \n\n");
    //aici incep cronometrarea pentru functiile de biblioteca POSIX
    clock_gettime(CLOCK_MONOTONIC, &start2); // measure wall clock time!
    pthread_barrier_init(&b2, NULL, NUMTHREADS);

    for(int i = 0;i<NUMTHREADS;i++){
        ids2[i] = i;
        pthread_create(&threads2[i],NULL,do_work2,(void *)&ids2[i]);
    }
    for(int i = 0;i<NUMTHREADS;i++){
        pthread_join(threads2[i],NULL);
    }

    pthread_barrier_destroy(&b2);
    clock_gettime(CLOCK_MONOTONIC, &finish2);
    elapsed2 = (finish2.tv_sec - start2.tv_sec);
    elapsed2 += (finish2.tv_nsec - start2.tv_nsec) / 1000000000.0;

    float diferenta = elapsed1 - elapsed2;
    printf("\n\n");
    printf("runtime pthread_thread_t este %f\n",elapsed2);
    printf("runtime my_thread_t este %f\n",elapsed1);
    printf("diferenta este %f\n",diferenta);
    return 0;
}