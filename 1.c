#include<stdio.h>
#include<stdlib.h>
#include <pthread.h>

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
typedef struct{
    int count;
    int thread_count;//numar numarul de thread-uri care au apelat functia my_barrier_wait
    int first;
}my_barrier_t;

my_barrier_t my_barrier;

void my_barrier_init(my_barrier_t *b, int count){
    b->count = count;
}

    //in aceasta functie ca trebui sa verific daca thread-ul care a apelat o
    //este ultimul thread
    //daca da, inseamna ca voi debloca toate celelalte thread-uri
    //daca nu, blochez thread-ul curent folosind mutex_lock

    //cred ca va trb sa folosesc variabile de conditie, deoarece
    //: thread A waits until it is signaled
    //(notified) by one of the incrementer threads that x reached the
   // value 

   /*
   ceva de genul
   if(b->count == THREAD_COUNT){

   }
   else{
   }
   */
pthread_mutex_t count_mutex;
pthread_cond_t count_thread_cv;

void my_barrier_wait(my_barrier_t *b, int id){
//    pthread_mutex_t count_mutex;
//    pthread_cond_t count_thread_cv;

    // pthread_mutex_init(&count_mutex,NULL);
    // pthread_cond_init(&count_thread_cv,NULL);


    pthread_mutex_lock(&count_mutex);
    b->thread_count++;
    while(b->thread_count < THREAD_COUNT){
        printf("threadul=%d asteapta sa se atinga valoarea %d\n",id,THREAD_COUNT);
        pthread_cond_wait(&count_thread_cv,&count_mutex);
        printf("threadul=%d a primit semnalul sa se reactiveze\n",id);
    }
    if(b->thread_count == THREAD_COUNT && b->first == 0){
        int n = pthread_cond_broadcast(&count_thread_cv);
        if(n != 0){
            perror("ceva s a intamplat\n");
            exit(-1);
        }
        printf("\n\n am dat broastcast la toate celelalte thread-uri, eu fiind thread-ul cu id=%d\n\n",id);
        b->first = 1;
        /*
        The  pthread_cond_broadcast()  function  shall  unblock all threads
        currently blocked on the specified condition variable cond.
        */
    }
    pthread_mutex_unlock(&count_mutex);
//    if(b->thread_count == THREAD_COUNT){
//     //inseamna ca s a ajuns la ultimul thread, care trb un semnal
//     //catre toate celelalte thread-uri sa se trezeaza
//     pthread_cond_signal(&count_thread_count_cv);//acesta este de fapt THREAD_COUNT-ul meu
//    }

   /*
   pthread_mutex_destroy(&count_mutex);
   pthread_cond_destroy(&count_threshold_cv);
   //desi cred ca toate aceste functii vor fi folosite in functie MEA de destroy, le lasam aici momentan
   */

}
void *functie(void *id){
    int thread_id = *(int*)id;
    printf("thread %d has started stage1 \n",thread_id);
    
    
    my_barrier_wait(&my_barrier,thread_id);

    printf("thread %d is going to stage 2\n",thread_id);


    return NULL;
}
int main(){

    pthread_t threads[THREAD_COUNT];
    int ids[THREAD_COUNT];

    my_barrier_init(&my_barrier,THREAD_COUNT);
    pthread_mutex_init(&count_mutex,NULL);
    pthread_cond_init(&count_thread_cv,NULL);

    for(int i = 0;i<THREAD_COUNT;i++){
        ids[i] = i;
        pthread_create(&threads[i],NULL,functie,(void *)&ids[i]);
    }
    for(int i = 0;i<THREAD_COUNT;i++){
        pthread_join(threads[i],NULL);
    }

    pthread_mutex_destroy(&count_mutex);
    pthread_cond_destroy(&count_thread_cv);
    return 0;
}