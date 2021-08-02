/*
Example 1: Printing threads without a semaphore. 
Expected output: sometimes thread 2 gets printed before thread 1. 

How to enforce thread printing order using a semaphore?
*/
#include <pthread.h>
#include <stdio.h>

void *f1(void * arg)
{
    printf("thread 1\n");
    pthread_exit(0);
}

void *f2(void * arg)
{
    printf("thread 2\n");
    pthread_exit(0);
}

int main()
{
    pthread_t myThread[2];
    pthread_create(&myThread[0], 0, f1, 0);
    pthread_create(&myThread[1], 0, f2, 0);
    pthread_join(myThread[0], 0);
    pthread_join(myThread[1], 0);
    return 0;
}
