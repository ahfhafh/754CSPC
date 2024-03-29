#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUMBER_OF_THREADS 3

void * thread_print(void * tid) 
{
	while (1) 
	{
		printf("thread %ld running\n", tid);
		sleep( 1);
	}
	pthread_exit(0);
}

int main() 
{
	pthread_t threads[NUMBER_OF_THREADS];
	for (long i = 0; i < NUMBER_OF_THREADS; i++) 
	{
		pthread_create(& threads[i], NULL, thread_print, (void *) i); 
	}
	
	sleep(5); // pretend to do something
	
	// here we would like to request the thread to be cancelled
	for (long i = 0; i < NUMBER_OF_THREADS; i++)
	{
		pthread_join(threads[i], NULL);
	}
	
	exit(0);
}