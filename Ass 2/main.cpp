#include <iostream>
#include <pthread.h>

using namespace std;
#define NUM_ITEMS 12
#define NUM_THREADS 3

struct Param
{
  int startIndex;
  int endIndex;
  int threadIndex;
};

int arr[NUM_ITEMS] = {23, 12, 56, 67, 22, 1, 4, 21, 10, 11, 7, 8};
int total[NUM_THREADS];

void *sum(void *a)
{
  struct Param *param = ((struct Param *)a);
  int end = (param->endIndex > NUM_ITEMS) ? NUM_ITEMS : param->endIndex;
  for (int i = param->startIndex; i < end; i++)
  {
    total[param->threadIndex] += arr[i];
  }

  pthread_exit(0);
}

int main()
{
  int workload = NUM_ITEMS / NUM_THREADS;

  struct Param param[NUM_THREADS];
  pthread_t threads[NUM_THREADS];

  for (int i = 0; i < NUM_THREADS; i++)
  {
    param[i].startIndex = i * workload;
    param[i].endIndex = param[i].startIndex + workload;
    param[i].threadIndex = i;

    pthread_create(&threads[i], NULL, sum, &param[i]);
  }

  for (int i = 0; i < NUM_THREADS; i++)
  {
    pthread_join(threads[i], NULL);
  }

  for (int i = 0; i < NUM_THREADS; i++)
  {
    cout << "Thread " << i << " total: " << total[i] << endl;
  }
}