#include<stdio.h>
#include<stdlib.h>
#include <pthread.h>
#include<unistd.h>
/*
trebuie implementate urmatoarele functii
void my_barrier_init(my_barrier_t *b, int count);

void my_barrier_destroy(my_barrier_t *b);

void my_barrier_wait(my_barrier_t *b);
*/

/*
In order to use your barrier in programs, one needs to declare a my_barrier_t variable and initialize it with 
my_barrier_init() - the function my_barrier_init() takes the number of threads that will be participating in the barrier as an argument.  

my_barrier_wait() - implements the synchronization point, each thread that participates in the barrier must call the function my_barrier_wait()
at the desired synchronization point. If the barrier has been initialized with count=N, then the first N-1 threads calling my_barrier_wait are blocked, 
until the last thread calls my_barrier_wait. After this, all threads of the barrier are unblocked and continue their execution.
The barrier is back at ready to be used again.

You can use pthreads  mutex locks and condition variables for your implementation. 
The barrier implementation will maintain a counter of threads reaching the barrier.  
This counter is a shared variable. A call to my_barrier_wait changes the counter, and if it is not the last thread,
it starts waiting for a signal that the last thread arrived. If the counter shows that it was the last thread, the signal is sent.
*/
#define THREAD_COUNT 10
#define THREAD_COUNT2 2
typedef struct my_barrier_t {
    int count;
    int thread_count;//numar numarul de thread-uri care au apelat functia my_barrier_wait
    int generation;
    //tu ai nevoie sa adaugi campurile : 
    /*
    pthread_mutex_t count_mutex;
    pthread_cond_t count_thread_cv;

    pentru cazul in care tu ai mai multe bariere
    */
    pthread_mutex_t count_mutex;
    pthread_cond_t count_thread_cv;

}my_barrier_t;

my_barrier_t my_barrier;
my_barrier_t my_barrier_2;

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

    //int current_generation = b->generation;//imi salvez generatia curenta

    pthread_mutex_lock(&(b->count_mutex));
    b->thread_count++;
    int current_generation = b->generation;//imi salvez generatia curenta, !! intr - regiune critica
    while (b->thread_count < b->count && current_generation == b->generation) {
        pthread_cond_wait(&(b->count_thread_cv), &(b->count_mutex));
    }
    if(b->thread_count == b->count){  //&& b->generatie[b->first] == 0){
        int n = pthread_cond_broadcast(&(b->count_thread_cv));
        if(n != 0){
            perror("ceva s a intamplat\n");
            exit(-1);
        }
        printf("\n\n  am dat broastcast la toate celelalte thread-uri\n\n");
        b->thread_count = 0;
        b->generation ++;
    }
    pthread_mutex_unlock(&(b->count_mutex));
}
void *functie(void *id){
    int thread_id = *(int*)id;
    printf("thread %d has started stage1 \n",thread_id);
    my_barrier_wait(&my_barrier);
    printf("thread %d is going to stage 2\n",thread_id);

    if(thread_id == 3){
        printf("thread 3 is ready to play football with thread 4\n");
        sleep(2);
        my_barrier_wait(&my_barrier_2);//cumva aici se asteapta unul pe celalalt sa ajunga acasa pentru a putea invata
        printf("thread 3 plays football with thread 4\n");
        //sleep(2);
    }

    if(thread_id == 4){
        printf("thread 4 is ready to play football with thread 3\n");
        sleep(2);
        my_barrier_wait(&my_barrier_2);//aici se asteapta unul pe celalalt pentru a se putea juca fotbal
        printf("thread 4 plays football with thread 3\n");
        //sleep(2);
    }
    return NULL;
}
int main(){

    pthread_t threads[THREAD_COUNT];
    int ids[THREAD_COUNT];

    my_barrier_init(&my_barrier,THREAD_COUNT);
    my_barrier_init(&my_barrier_2,THREAD_COUNT2);

    for(int i = 0;i<2;i++){
        printf("\n\n iteratia %d \n\n",i);
        for(int i = 0;i<THREAD_COUNT;i++){
        ids[i] = i;
        pthread_create(&threads[i],NULL,functie,(void *)&ids[i]);
    }
    for(int i = 0;i<THREAD_COUNT;i++){
        pthread_join(threads[i],NULL); 
    }
    }

    // for(int i = 0;i<THREAD_COUNT;i++){
    //     ids[i] = i;
    //     pthread_create(&threads[i],NULL,functie,(void *)&ids[i]);
    // }
    // for(int i = 0;i<THREAD_COUNT;i++){
    //     pthread_join(threads[i],NULL); 
    // }
    my_barrier_destroy(&my_barrier);
    my_barrier_destroy(&my_barrier_2);
    // pthread_mutex_destroy(&count_mutex);
    // pthread_cond_destroy(&count_thread_cv);
    return 0;
}
/*
intrebari:
1) ar trb sa nu am declarate global ??
pthread_mutex_t count_mutex;
pthread_cond_t count_thread_cv;

si cumva sa le introduc in structura my_barrier_t ca niste campuri ?

2)nu stiu exact ce ar trebui sa se intample in functia 
my_barrier_destroy(my_barrier_t *b)
daca folosesc alocare dinamica, stiu ce trebuia sa fac, sa dau free,
dar asa, nu stiu exact ce as putea sa fac
*/


// typedef struct{
//     char nume[10];
//     int status_home;//status_home == 0, inseamna ca membrul familiei nu a ajuns acasa
//     //status_home == 1, inseamna ca membrul familiei nu a ajuns acasa
// }membru_familie;

// membru_familie m[THREAD_COUNT];

// void initializare_membru(int id, membru_familie *m){
//     if(id == 0){
//         strcpy((m)->nume,"Mama");
//     }else if(id == 1){
//         strcpy((m)->nume,"Papa");
//     }else if(id == 2){
//         strcpy((m)->nume,"Alice");
//     }else if(id == 3){
//         strcpy((m)->nume,"Bob");
//     }else if(id == 4){
//         strcpy((m)->nume,"Chris");
//     }else if(id == 5){
//         strcpy((m)->nume,"Dave");
//     }
//     (m)->status_home = 0;

//     //intrebare Petra
//     //eu in functia asta voiam sa folosesc peste tot (&m)
//     //adica toate valorile sa se puna la adresa de memorie
//     //asta de ce nu era bine ?
//     //primeam eroarea :  error: ‘m’ is a pointer; did you mean to use ‘->’?

// }