/*
Example: Use 18 threads to read 18 numbers from a file. 
*/
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cinttypes>
#include <cmath>
#include <iostream>
#include <pthread.h>

using namespace std;

pthread_mutex_t myMutex;

bool read(int64_t *num) {
    if( ! (cin >> *num) )
        return false;
    else
        return true; 
}

void * thread_task (void * i)
{
    int64_t num;
    intptr_t id = (intptr_t) i;
    bool res = false;
    
    // protect the read section
    pthread_mutex_lock(&myMutex);
    res = read(&num);
    pthread_mutex_unlock(&myMutex);
    
    if (res)
    {
        pthread_mutex_lock(&myMutex);
        cout << "Thread #" << id << " read number " << num << endl;
        pthread_mutex_unlock(&myMutex);
    }

    pthread_exit(0);
}

int main( int argc, char ** argv)
{
    /// parse command line arguments
    int nThreads = 5;

    // create threads
    pthread_t threadPool[nThreads];
    for (int i = 0; i < nThreads; i++)
    {
        pthread_create(&threadPool[i], NULL, thread_task, (void *)i);
    }

    // program done, join all threads and end.
    for (int i = 0; i < nThreads; i++)
    {
        pthread_join(threadPool[i], NULL);
    }
    
    cout << "done!" << endl;
    return 0;
}