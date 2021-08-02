/*
Example 2: Using barrier to recycle threads. 

Xining Chen

compile with:
    $ g++ barrierExp2.cpp -O3 -o b2 -lm -lpthread

run with:
    $ ./b2 < test1.txt

test1.txt should be a file with a bunch of integers. Can use easy.txt from A3 test files. 
*/
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cinttypes>
#include <cmath>
#include <iostream>
#include <pthread.h>

using namespace std;

pthread_barrier_t myBarrier;
pthread_mutex_t myMutex;
int64_t num;
bool readSuc;

void thread_task (void * i)
{
    intptr_t id = (intptr_t) i;
    pthread_mutex_lock(&myMutex);
    cout << "Thread #" << id << " working on number " << num << endl;
    pthread_mutex_unlock(&myMutex);
}

bool read() {
    if( ! (std::cin >> num) )
        return false;
    else
        return true; 
}

void * thread_start(void * tid) {
    //bool readSuc = false;
    cout << "Thread #" << (intptr_t) tid << " started!" << endl;
    while(1) {
        // synchronize all threads
        int res = pthread_barrier_wait(&myBarrier);
        if( res == PTHREAD_BARRIER_SERIAL_THREAD)
        {
            cout << "Single thread - run serial work" << endl;
            pthread_mutex_lock(&myMutex);
            readSuc = read();
            pthread_mutex_unlock(&myMutex);
            cout << "finished reading: "<< readSuc << endl;
        }
     
        // synchronize again before starting parallel work
        pthread_barrier_wait(&myBarrier);   

        // run thread_task in parallel 
        if (readSuc == true)        // if read was successful, run threads
        {
            thread_task(tid);
        }
        else
        {
            pthread_mutex_lock(&myMutex);
            cout << "EXITING!" << endl;
            pthread_mutex_unlock(&myMutex);

            break;
        }
    }

    pthread_exit(0);
}

int main( int argc, char ** argv)
{
    /// parse command line arguments
    int nThreads = 5;
    pthread_barrier_init(&myBarrier, NULL, nThreads);

    // create threads
    pthread_t threadPool[nThreads];
    for (int i = 0; i < nThreads; i++)
    {
        pthread_create(&threadPool[i], NULL, thread_start, (void *)i);
    }

    // program done, join all threads and end.
    for (int i = 0; i < nThreads; i++)
    {
        pthread_join(threadPool[i], NULL);
    }
    
    cout << "done!" << endl;
    return 0;
}

