#include<stdio.h>
#include<stdlib.h>
#include <pthread.h>
#include<string.h>
#include<unistd.h>

#define THREAD_COUNT 6
#define THREAD_COUNT_STUDY 2
#define THREAD_COUNT_PLAY_FOOTBAL 2
/*
Demonstrate the use of your barrier implementation. Use several variables of my_barrier_t type: 

Create a multithreaded model of the  daily life of Jones Family: 
Mama Jones, Papa Jones, Alice, Bob, Chris and Dave.  
Every member of the family is represented by a thread.
Each thread knows the name of the family member it is representing and 
executes its daily schedule.
Every family member has specific individual activities but there are also
activities that require the synchronization of the whole family or just
a part of the family members. 
Use your implementation of a barrier to ensure these synchronization 
points.

The daily schedule of the Jones is the following:   
In the morning after waking up, all wait for each other to have breakfast together. 
After this, Mama and Papa go to work and start working at their jobs. 
Alice,Bob, Chris and Dave go to school,  
each studies in his/her class their specific subjects  
and after that they go home, each kid walks home alone when he finishes his classes. 
At home, Alice and Bob wait for each other to start studying math together.  
Chris and Dave wait for each other to  play football together. 
After this, all the children wait for their parents to have dinner together. 
Everyone gets to sleep until next morning. 
Let your threads simulate the life of the Jones for 5 days, Monday-Friday.
The threads will simulate activities by printing messages for the start and end of an activity, 
as well as messages when they are ready to start group activities.
*/
/*
Mama Jones = thread0
Papa Jones = thread1
Alice = thread2
Bob = thread3
Chris = thread4
Dave = thread5
*/

/*
E1)la crearea thread-urilor afisam mesajul de, "nume_persoana woke up"
E2)In the morning after waking up, all wait for each other to have breakfast together. - aici introducem o bariera, pentru ca toti membrii familiei sa manance impreuna
E3)After this, Mama and Papa go to work and start working at their jobs. 
Alice,Bob, Chris and Dave go to school, 
E4) At home, Alice and Bob wait for each other to start studying math together. 
E5)Chris and Dave wait for each other to  play football together. 
E6)After this, all the children wait for their parents to have dinner together. 
*/
pthread_barrier_t b_breakfast;//bariera prin intermediul careia toti asteapta unii dupa altii sa aiba breakfast
pthread_barrier_t b_study;//bariera prin care Alice si Bob asteapta sa studieze impreuna
pthread_barrier_t b_play_footbal;//bariera prin care Chris si Dave asteapta sa joace fotbal impreuna
pthread_barrier_t b_dinner;//bariera prin care toti membrii familiei asteapta sa manance cina impreuna

char *cast_id_nume(int id){
    char *nume;
    nume = (char *)malloc(10*sizeof(char));
    if(nume == NULL){
        perror("eroare alocare dinamica\n");
        exit(-1);
    }
    if(id == 0){
        strcpy(nume,"Mama");
    }else if(id == 1){
        strcpy(nume,"Papa");
    }else if(id == 2){
        strcpy(nume,"Alice");
    }else if(id == 3){
        strcpy(nume,"Bob");
    }else if(id == 4){
        strcpy(nume,"Christ");
    }else if(id == 5){
        strcpy(nume,"Dave");
    }

    return nume;
}
void *functie(void *id){
    int thread_id = *(int *)id;
    char *nume = cast_id_nume(thread_id);
    printf("%s woke up\n",nume);


    printf("%s is waiting  to have breakfast\n",nume);
    pthread_barrier_wait(&b_breakfast);

    if(thread_id == 0 || thread_id == 1){
            printf("%s is going to work\n",nume);
            sleep(10);
    }else{
        printf("%s is going to school\n",nume);
        sleep(3);
    }
    if(thread_id != 0 && thread_id != 1){
        printf("%s has finished school and is going home now\n",nume);
        sleep(1);
    }
    if(thread_id == 2){
        printf("%s is ready to learn math with Bob\n",nume);
        pthread_barrier_wait(&b_study);//cumva aici se asteapta unul pe celalalt sa ajunga acasa pentru a putea invata
        printf("%s learns math with Bob\n",nume);
        sleep(2);
    }
    if(thread_id == 3){
        printf("%s is ready to learn math with Alice\n",nume);
        pthread_barrier_wait(&b_study);//cumva aici se asteapta unul pe celalalt sa ajunga acasa pentru a putea invata
        printf("%s learns math with Alice\n",nume);
        sleep(2);
    }

    if(thread_id == 4){
        printf("%s is ready to play football with Dave\n",nume);
        pthread_barrier_wait(&b_play_footbal);//aici se asteapta unul pe celalalt pentru a se putea juca fotbal
        printf("%s plays football with Dave\n",nume);
        sleep(2);
    }
    if(thread_id == 5){
        printf("%s is ready to play football with Chris\n",nume);
        pthread_barrier_wait(&b_play_footbal);//aici se asteapta unul pe celalalt pentru a se putea juca fotbal
        printf("%s plays football Chris\n",nume);
        sleep(2);
    }
    if(thread_id == 0 || thread_id == 1){
        printf("%s has arrived home \n",nume);
        sleep(1);
    }
    printf("%s is waiting  to have dinner\n",nume);
    pthread_barrier_wait(&b_dinner);
    return NULL;
}
int main(){

    pthread_t threads[THREAD_COUNT];
    int ids[THREAD_COUNT];
    char saptamana[100][100] = {"Luni", "Marti", "Miercuri", "Joi","Vineri"};

    pthread_barrier_init(&b_breakfast,NULL,THREAD_COUNT);
    pthread_barrier_init(&b_study,NULL,THREAD_COUNT_STUDY);//2 deoaree
    pthread_barrier_init(&b_play_footbal,NULL,THREAD_COUNT_PLAY_FOOTBAL);//2 deoaree
    pthread_barrier_init(&b_dinner,NULL,THREAD_COUNT);//2 deoaree

    for(int j = 0;j<5;j++){
        printf("\n%s\n",saptamana[j]);
        for(int i = 0;i<THREAD_COUNT;i++){
            ids[i] = i;
            pthread_create(&threads[i],NULL,functie,(void*)&ids[i]);
        }
        for(int i = 0;i<THREAD_COUNT;i++){
            pthread_join(threads[i],NULL);
        }
        pthread_barrier_destroy(&b_breakfast);
        pthread_barrier_destroy(&b_study);
        pthread_barrier_destroy(&b_play_footbal);
        pthread_barrier_destroy(&b_dinner);
        }
    return 0;
}