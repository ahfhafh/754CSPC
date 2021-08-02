/* Example 1 of using barrier to synchronize 5 threads. 
 * What does the output look like?
 *
 * Try removing all the barrier code. What do you expect the output to be like?
 * What is the output after barrier is removed?
 */

#include <pthread.h>
#include <stdio.h>
#include <iostream>

using namespace std;
pthread_barrier_t myBarrier;
pthread_mutex_t myMutex;

// thread function
void *task(void *i)
{
    intptr_t id = (intptr_t)i;

    pthread_mutex_lock(&myMutex);
    cout << "before the barrier " << id << endl;
    pthread_mutex_unlock(&myMutex);

    // calls barrier wait to wait for other threads to reach this barrier point
    pthread_barrier_wait(&myBarrier);

    pthread_mutex_lock(&myMutex);
    cout << "after the barrier " << id << endl;
    pthread_mutex_unlock(&myMutex);
}

int main()
{
    int nThread = 5;

    // create a barrier for total number of threads.
    pthread_barrier_init(&myBarrier, NULL, nThread);

    // create 5 threads
    pthread_t threads[nThread];
    for (int i = 0; i < nThread; i++)
        pthread_create(&threads[i], NULL, task, (void *)i);

    // join all threads
    for (int i = 0; i < nThread; i++)
        pthread_join(threads[i], NULL);

    // destroy barrier
    pthread_barrier_destroy(&myBarrier);
    return 0;
}
