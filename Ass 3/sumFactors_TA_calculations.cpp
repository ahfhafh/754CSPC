/**********************************************
 * Last Name:   Yang
 * First Name:  Jack
 * Student ID:  30062393
 * Course:      CPSC 457
 * Tutorial:    T05
 * Assignment:  3
 *
 * File name: sumFactors.cpp
 *********************************************/
/// compile with:
///   $ g++ sumFactors.cpp -O3 -o gcd -lm

#include <stdio.h>
#include <vector>
#include <pthread.h>
#include <queue>
#include <atomic>
#include <algorithm>
#include <semaphore.h>
#include <math.h>

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cinttypes>
#include <cmath>
#include <iostream>

// create mutex
pthread_mutex_t mtx;
// create semaphore
sem_t sem;
// create barrier
pthread_barrier_t barr;
// create queue
std::queue<int64_t> queue;
// number of threads
int64_t num_threads;
// thread cancelation flag
std::atomic<bool> cnl_flag(false);
// struct for each thread
struct Param
{
  int64_t i;
  int64_t max;
  int64_t tid;
};
// store potential results
std::vector<int16_t> results;
/// sum up the divisors
int64_t sum = 0;
/// calculates smallest divisor of n that is less than n
///
/// returns 0 if n is prime or n < 2
///
/// examples:
///    n =  1         --> result = 0
///    n =  2         --> result = 0
///    n =  9         --> result = 3
///    n = 35         --> result = 5
///    n = 8589934591 --> result = 7
void *getSmallestDivisor(void *arg)
{
  struct Param *param = ((struct Param *)arg);
  while (1)
  {
    int64_t res;
    pthread_mutex_lock(&mtx);
    // grab next number in queue
    int64_t n = queue.front();
    pthread_mutex_unlock(&mtx);

    if (cnl_flag == true)
    {
      // wait for threads after finding 1 or no divisor
      res = pthread_barrier_wait(&barr);

      if (res == PTHREAD_BARRIER_SERIAL_THREAD)
      {
        std::cout << "div: " << *min_element(results.begin(), results.end())
                  << "\n";

        // add smallest element in results
        sum += *min_element(results.begin(), results.end());
        results.clear();
        cnl_flag = false;

        std::cout << "sum: " << sum << "\n";
      }

      pthread_mutex_lock(&mtx);
      // out of numbers to calculate
      if (queue.empty())
      {
        std::cout << "\nWAITING TO END------------------------" << param->tid << "------------------------- "
                  << "\n";
        pthread_mutex_unlock(&mtx);
        break;
      }
      pthread_mutex_unlock(&mtx);
    }
    pthread_barrier_wait(&barr);
    if (param->tid == 0)
      queue.pop();

    pthread_mutex_lock(&mtx);
    std::cout << "STARTING----" << param->tid << "----: " << n << "--------------------------------------------------------------------------------------------\n";
    pthread_mutex_unlock(&mtx);

    // synchronize
    res = pthread_barrier_wait(&barr);
    // run single thread
    if (res == PTHREAD_BARRIER_SERIAL_THREAD)
    {
      if (n <= 3)
      {
        results.push_back(0); // 2 and 3 are primes
        cnl_flag = true;
        for (int64_t i = 0; i < num_threads - 1; i++)
          sem_post(&sem);
        continue;
      }
      if (n % 2 == 0)
      {
        results.push_back(2); // handle multiples of 2
        cnl_flag = true;
        for (int64_t i = 0; i < num_threads - 1; i++)
          sem_post(&sem);
        continue;
      }
      if (n % 3 == 0)
      {
        results.push_back(3); // handle multiples of 3
        cnl_flag = true;
        for (int64_t i = 0; i < num_threads - 1; i++)
          sem_post(&sem);
        continue;
      }
      for (int64_t i = 0; i < num_threads; i++)
        sem_post(&sem);
    }
    sem_wait(&sem);
    // cancellation point
    if (cnl_flag == true)
    {
      pthread_mutex_lock(&mtx);
      std::cout << "CANCEL NOW-----" << param->tid << "----: " << n << "\n";
      pthread_mutex_unlock(&mtx);
      continue;
    }
    // synchronize
    res = pthread_barrier_wait(&barr);

    pthread_mutex_lock(&mtx);
    std::cout << "DOING----" << param->tid << "-----: " << n << "\n";
    pthread_mutex_unlock(&mtx);
    // start parallel work -------------------------------------------------------------
    // caluclate lower and upper bounds for each thread

    // run single thread
    if (res == PTHREAD_BARRIER_SERIAL_THREAD)
    {
      int64_t start = 5;
      int64_t max = sqrt(n);
      int64_t diff = max - start;
      int64_t num = diff / (6 * num_threads);
      for (int i = 0; i < num_threads; i++)
      {
        int64_t ts = start;
        int64_t te = ts + (6 * num);
        if (i == (num_threads - 1))
          te += (max - te);
        else
          start = te;
        param[i].i = ts;
        param[i].max = te;
        std::cout << "Thread --> " << i << "    ts --> " << ts << "    te --> " << te << std::endl;
      }
    }

    // Synchronize
    pthread_barrier_wait(&barr);

    pthread_mutex_lock(&mtx);
    //std::cout << "Num: " << n << "     Thread: " << param->tid << "   Lower Bound: " << param->i << "       Upper Bound: " << param->max << std::endl;
    pthread_mutex_unlock(&mtx);

    while (param->i <= param->max)
    {
      if (cnl_flag == true)
        break;
      if (n % param->i == 0)
      {
        results.push_back(param->i);
        cnl_flag = true;
        break;
      }
      if (n % (param->i + 2) == 0)
      {
        results.push_back(param->i + 2);
        cnl_flag = true;
        break;
      }
      param->i += 6;
    }
  }
  pthread_exit(0);
}

int main(int argc, char **argv)
{
  /// parse command line arguments
  int nThreads = 1;
  if (argc != 1 && argc != 2)
  {
    std::cout << "Usage: " << argv[0] << " [nThreads]\n";
    exit(-1);
  }
  if (argc == 2)
    nThreads = atoi(argv[1]);

  /// handle invalid arguments
  if (nThreads < 1 || nThreads > 256)
  {
    std::cout << "Warning: Bad arguments. 1 <= nThreads <= 256!\n";
  }
  /*
  if (nThreads != 1)
  {
    std::cout << "Error: I am not multithreaded yet :-(\n";
    exit(-1);
  }
  */

  // update global variable for number of threads
  num_threads = nThreads;
  std::cout << "Using " << nThreads
            << " thread" << (nThreads == 1 ? "" : "s")
            << ".\n";

  // calculate divisors and sum of divisors
  while (1)
  {
    int64_t num;
    if (!(std::cin >> num))
      break;
    queue.push(num);

    // the line below may help you with debugging
    // std::cout << "Debug: " << num << " --> " << div << "\n";
    // sum += div;
  }

  // create threads
  pthread_t threads[nThreads];
  // create struct for threads
  struct Param param[nThreads];

  // create a barrier for total number of threads.
  pthread_barrier_init(&barr, NULL, nThreads);
  // create semaphore
  sem_init(&sem, 0, 0);

  for (int64_t i = 0; i < nThreads; i++)
  {
    if (0 != pthread_create(&threads[i], NULL, getSmallestDivisor, &param[i]))
    {
      std::cout << "Thread create failed\n";
      exit(-1);
    }
    param[i].tid = i;
  }

  std::cout << "\nJoin Threads\n";
  // join threads
  for (int64_t i = 0; i < nThreads; i++)
  {
    pthread_join(threads[i], NULL);
  }

  std::cout << "\nAfter Join Threads\n";

  // destroy barrier
  pthread_barrier_destroy(&barr);

  /// report results
  std::cout << "Sum of divisors = " << sum << "\n";

  return 0;
}
