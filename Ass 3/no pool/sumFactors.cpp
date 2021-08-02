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
// number of threads
int64_t num_threads;
// thread cancelation flag
std::atomic<bool> cnl_flag(false);
// store potential results
std::vector<int64_t> results;
/// sum up the divisors
int64_t sum = 0;
// struct for each thread
struct Param
{
  int64_t i, max, tid;
};
int64_t num;

// part 1 of the algorithm
bool do_part_one(int64_t n)
{
  if (n <= 3)
  {
    results.push_back(0); // 2 and 3 are primes
    cnl_flag = true;
    for (int64_t i = 0; i < num_threads - 1; i++)
      sem_post(&sem);
    return true;
  }
  if (n % 2 == 0)
  {
    results.push_back(2); // handle multiples of 2
    cnl_flag = true;
    for (int64_t i = 0; i < num_threads - 1; i++)
      sem_post(&sem);
    return true;
  }
  if (n % 3 == 0)
  {
    results.push_back(3); // handle multiples of 3
    cnl_flag = true;
    for (int64_t i = 0; i < num_threads - 1; i++)
      sem_post(&sem);
    return true;
  }
  for (int64_t i = 0; i < num_threads; i++)
    sem_post(&sem);
  return false;
}

// part 2 of the algorithm
void do_part_two(void *args, int64_t n)
{
  struct Param *param = ((struct Param *)args);

  while (param->i <= param->max)
  {
    if (n % param->i == 0)
    {
      pthread_mutex_lock(&mtx);
      results.push_back(param->i);
      pthread_mutex_unlock(&mtx);
      return;
    }
    if (n % (param->i + 2) == 0)
    {
      pthread_mutex_lock(&mtx);
      results.push_back(param->i + 2);
      pthread_mutex_unlock(&mtx);
      return;
    }
    param->i += 6;
  }
  return;
}
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

  // Calculations start here-------------------------------------------------------------------------------------------------------------------------------------
  // synchronize
  int64_t res = pthread_barrier_wait(&barr);
  // run single thread
  if (res == PTHREAD_BARRIER_SERIAL_THREAD)
    if (do_part_one(num))
      pthread_exit(0);

  sem_wait(&sem);

  // cancellation point
  if (cnl_flag == true)
    pthread_exit(0);

  // synchronize
  pthread_barrier_wait(&barr);

  // start parallel work -------------------------------------------------------------
  // caluclate lower and upper bounds for each thread
  param->i = (sqrt(num) < (5 + floor((floor(sqrt(num)) - 5) / (6 * num_threads)) * param->tid * 6) || ((5 + floor((floor(sqrt(num)) - 5) / (6 * num_threads)) * param->tid * 6) < 5)) ? 5 : (5 + floor((floor(sqrt(num)) - 5) / (6 * num_threads)) * param->tid * 6);
  param->max = ((5 + floor((floor(sqrt(num)) - 5) / (6 * num_threads)) * (param->tid + 1) * 6) > sqrt(num)) ? sqrt(num) : (5 + floor((floor(sqrt(num)) - 5) / (6 * num_threads)) * (param->tid + 1) * 6);
  if (param->tid == num_threads - 1)
    if (param->max != sqrt(num))
      param->max = sqrt(num);

  do_part_two(param, num);

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
    if (!(std::cin >> num))
      break;
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

    // join threads
    for (int64_t i = 0; i < nThreads; i++)
    {
      pthread_join(threads[i], NULL);
    }

    // destroy barrier
    pthread_barrier_destroy(&barr);
    // destory semaphore
    sem_destroy(&sem);

    // add smallest element in results
    if (!results.empty())
    {
      sum += *min_element(results.begin(), results.end());
      results.clear();
    }
    cnl_flag = false;
  }
  /// report results
  std::cout << "Sum of divisors = " << sum << "\n";

  return 0;
}