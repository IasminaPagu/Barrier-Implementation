/* File:
 *    condvar.c
 * It illustrates the use of Condition Variables for threads synchronization
 * Main creates a number of NUM_THREADS=3 threads
 * There is a shared variable count, initialized with 0
 * The first thread waits until count reaches the value THRESHOLD=10
 * The next two threads increment count, each a number of REPEAT=15 times.
 * The incrementer thread that sees that count has reached the THRESHOLD
 * signals the condition in order to wake up the first thread
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 3
#define REPEAT 15
#define THRESHOLD 10

/* shared variable */
int count = 0;

/* mutex to protect critical regions
when threads are updating count */
pthread_mutex_t count_mutex;

/* condition variable to signal
when count reaches threshold value*/
pthread_cond_t count_threshold_cv;

/* thread function for incrementer threads */
void *increment_count(void *t)
{
  int i;
  int my_id = *(int *)t;
  sleep(1); /* simulate some activity */
  for (i = 0; i < REPEAT; i++)
  {
    pthread_mutex_lock(&count_mutex);
    count++;
    printf("increment_count(): thread %d, count = %d \n", my_id, count);
    if (count == THRESHOLD)
    {
      /* if condition reached send signal to waiting thread  */
      printf("increment_count(): thread %d, count = %d  Threshold reached! Send signal \n",
             my_id, count);
      pthread_cond_signal(&count_threshold_cv);
    }
    pthread_mutex_unlock(&count_mutex);
    sleep(1); /* simulate some work*/
  }
  pthread_exit(NULL);
}

/* thread function for waiting thread */
void *wait_count(void *t)
{
  int my_id = *(int *)t;

  printf("Starting wait_count(): thread %ld\n", my_id);
  /*
  Lock mutex and wait for signal.
  Function pthread_cond_wait will automatically unlock mutex while it waits.
  While loop needed in case the thread gets unblocked by some other OS event!!!
  */
  pthread_mutex_lock(&count_mutex);
  while (count < THRESHOLD)
  {
    printf("wait_count(): thread %d Count= %d. Going into wait...\n", my_id, count);
    pthread_cond_wait(&count_threshold_cv, &count_mutex);
    printf("wait_count(): thread %d Condition signal received. Count= %d\n", my_id, count);
  }
  printf("wait_count(): thread %d Unlocking mutex.\n", my_id);
  pthread_mutex_unlock(&count_mutex);
  for (int i = 0; i < REPEAT; i++)
  {
    printf("wait_count(): thread %d also working \n", my_id);
    sleep(1); /* simulate work */
  }
  pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
  int i;
  pthread_t threads[NUM_THREADS];
  int ids[NUM_THREADS];

  /* Initialize mutex and condition variable objects */
  pthread_mutex_init(&count_mutex, NULL);
  pthread_cond_init(&count_threshold_cv, NULL);

  ids[0] = 0;
  pthread_create(&threads[0], NULL, wait_count, (int *)&ids[0]);
  for (i = 1; i < NUM_THREADS; i++)
  {
    ids[i] = i;
    pthread_create(&threads[i], NULL, increment_count, (int *)&ids[i]);
  }

  /* Wait for all threads to complete */
  for (i = 0; i < NUM_THREADS; i++)
  {
    pthread_join(threads[i], NULL);
  }
  printf("Main(): Waited and joined with %d threads. Final value of count = %d. Done.\n",
         NUM_THREADS, count);

  /* Clean up and exit */
  pthread_mutex_destroy(&count_mutex);
  pthread_cond_destroy(&count_threshold_cv);
  pthread_exit(NULL);
}