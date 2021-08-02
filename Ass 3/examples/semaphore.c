#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

#define NITER 1000000

int cnt = 0;
sem_t sem;

void * Count(void * a)
{
    int i, tmp;
    for(i = 0; i < NITER; i++)
    {
		sem_wait(&sem);
        tmp = cnt;
        tmp = tmp+1;
        cnt = tmp;      
		sem_post (&sem);
    }
	pthread_exit(NULL);
}

int main(int argc, char * argv[])
{
	sem_init(&sem, 0, 1);
	
    pthread_t tid1, tid2;

    pthread_create(&tid1, NULL, Count, NULL);
    pthread_create(&tid2, NULL, Count, NULL);    

    pthread_join(tid1, NULL);  
    pthread_join(tid2, NULL);    

    if (cnt < 2 * NITER) 
        printf("\n BOOM! cnt is [%d], should be %d\n", cnt, 2*NITER);
    else
        printf("\n OK! cnt is [%d]\n", cnt);
  
    return 0;
}

