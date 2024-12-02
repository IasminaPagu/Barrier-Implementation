In this assignment I implemented my own barrier synchronization primitive. My barrier implementation has implemented an API very similar to pthreads:
pthread_barrier_t  --  my_barrier_t

pthread_barrier_init -- my_barrier_init

pthread_barrier_destroy -- my_barrier_destroy

pthread_barrier_wait -- my_barrier_wait

The function my_barrier_init() takes the number of threads that will be participating in the barrier as an argument. 
The  function my_barrier_wait() implements the synchronization point. Each thread that participates in the barrier must call the function my_barrier_wait()  at the desired synchronization point.
If the barrier has been initialized with count=N, then the first N-1 threads calling my_barrier_wait are blocked, until the last thread calls my_barrier_wait.
After this, all threads of the barrier are unblocked and continue their execution. The barrier is back at ready to be used again.

Next, I created a multithreaded model of the  daily life of Jones Family: Mama Jones, Papa Jones, Alice, Bob, Chris and Dave, using my own implementation, demonstrating its functionality. 
Every member of the family is represented by a thread. Each thread knows the name of the family member it is representing and executes its daily schedule. 
Every family member has specific individual activities but there are also activities that require the synchronization of the whole family or just a part of the family members. 


