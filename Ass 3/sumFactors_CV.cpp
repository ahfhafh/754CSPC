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
int value;

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

pthread_cond_t cond;

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

std::atomic<bool> ez_flag(false);
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
bool firstpart(int64_t n)
{
  if (n <= 3)
  {
    results.push_back(0); // 2 and 3 are primes
    cnl_flag = true;
    ez_flag = true;
    pthread_cond_broadcast(&cond);
    return true;
  }
  if (n % 2 == 0)
  {
    results.push_back(2); // handle multiples of 2
    cnl_flag = true;
    ez_flag = true;
    pthread_cond_broadcast(&cond);
    return true;
  }
  if (n % 3 == 0)
  {
    results.push_back(3); // handle multiples of 3
    cnl_flag = true;
    ez_flag = true;
    pthread_cond_broadcast(&cond);
    return true;
  }
  ez_flag = true;
  pthread_cond_broadcast(&cond);
  return false;
}

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

    // found a divisor including 0, 2, or 3
    if (cnl_flag == true)
    {
      // wait for threads after finding 1 or no divisor
      res = pthread_barrier_wait(&barr);
      // use single thread
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

    // wait for all threads so another doesn't start before a pop
    pthread_barrier_wait(&barr);
    if (param->tid == 0)
      queue.pop();

    // Calculations start here-------------------------------------------------------------------------------------------------------------------------------------
    pthread_mutex_lock(&mtx);
    std::cout << "STARTING----" << param->tid << "----: " << n << "--------------------------------------------------------------------------------------------\n";
    pthread_mutex_unlock(&mtx);

    // synchronize
    res = pthread_barrier_wait(&barr);
    // run single thread
    if (res == PTHREAD_BARRIER_SERIAL_THREAD)
    {
      if (firstpart(n))
        continue;
    }

    while (ez_flag == false)
      pthread_cond_wait(&cond, &mtx);

    // cancellation point
    if (cnl_flag == true)
      continue;

    // synchronize
    pthread_barrier_wait(&barr);

    // start parallel work -------------------------------------------------------------
    // caluclate lower and upper bounds for each thread
    pthread_mutex_lock(&mtx);
    param->i = (5 + (floor(sqrt(n)) / (6 * num_threads)) * param->tid * 6);
    param->max = (5 + (floor(sqrt(n)) / (6 * num_threads)) * (param->tid + 1) * 6);
    std::cout << "Num: " << n << "     Thread: " << param->tid << "   Lower Bound: " << param->i << "       Upper Bound: " << param->max << std::endl;
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

  pthread_mutex_init(&mtx, 0);
  pthread_cond_init(&cond, 0);

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

  // join threads
  for (int64_t i = 0; i < nThreads; i++)
  {
    pthread_join(threads[i], NULL);
  }

  // destroy barrier
  pthread_barrier_destroy(&barr);
  // destory semaphore
  sem_destroy(&sem);

  /// report results
  std::cout << "Sum of divisors = " << sum << "\n";

  return 0;
}