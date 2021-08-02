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

// create CV
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
// counter for threads
std::atomic<int64_t> tcount;
// store potential results
std::vector<int64_t> results;
/// sum up the divisors
int64_t sum = 0;
// struct for each thread
struct Param
{
  int64_t i, max, tid;
};

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
  tcount = 0;
  pthread_barrier_wait(&barr);
  while (param->i <= param->max)
  {
    if (n % param->i == 0)
    {
      pthread_mutex_lock(&mtx);
      results.push_back(param->i);
      cnl_flag = true;
      tcount++;
      pthread_cond_broadcast(&cond);
      pthread_mutex_unlock(&mtx);
      return;
    }
    if (n % (param->i + 2) == 0)
    {
      pthread_mutex_lock(&mtx);
      results.push_back(param->i + 2);
      cnl_flag = true;
      tcount++;
      pthread_cond_broadcast(&cond);
      pthread_mutex_unlock(&mtx);
      return;
    }
    param->i += 6;
  }
  // count threads that pass this point
  tcount++; //atomic count
  pthread_mutex_lock(&mtx);
  // if all threads reach this point, release other threads
  if (tcount >= num_threads)
    pthread_cond_broadcast(&cond);
  pthread_mutex_unlock(&mtx);

  pthread_mutex_lock(&mtx);
  // wait for all threads or signal
  while (tcount < num_threads)
    pthread_cond_wait(&cond, &mtx);
  pthread_mutex_unlock(&mtx);

  // if no divisor has been found yet return 0
  if (param->tid == 0 && cnl_flag == false)
  {
    results.push_back(0);
    cnl_flag = true;
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
  while (1)
  {
    // synchronize
    pthread_barrier_wait(&barr);
    pthread_mutex_lock(&mtx);
    int64_t res;
    int64_t n = 0;
    // grab next number in queue
    if (!queue.empty())
      n = queue.front();
    pthread_mutex_unlock(&mtx);

    // found a divisor including 0, 2, or 3
    if (cnl_flag == true)
    {
      // wait for threads after finding 1 or no divisor
      res = pthread_barrier_wait(&barr);
      // use single thread
      if (res == PTHREAD_BARRIER_SERIAL_THREAD)
      {
        // add smallest element in results
        sum += *min_element(results.begin(), results.end());
        // clear potential results for current n
        results.clear();
        cnl_flag = false;
      }

      // out of numbers to calculate
      if (queue.empty())
        break; // Exit
    }

    // wait for all threads so another doesn't start before a pop
    res = pthread_barrier_wait(&barr);
    if (param->tid == 0 && !queue.empty())
      queue.pop(); // pop once

    // Calculations start here-------------------------------------------------------------------------------------------------------------------------------------
    // run single thread
    if (res == PTHREAD_BARRIER_SERIAL_THREAD)
      if (do_part_one(n))
        continue;

    // wait for before cnl_flag check
    sem_wait(&sem);

    // cancellation point
    if (cnl_flag == true)
      continue; // 0, 2, or 3 found

    // start parallel work -------------------------------------------------------------
    // caluclate lower and upper bounds for each thread
    param->i = (sqrt(n) < (5 + floor((floor(sqrt(n)) - 5) / (6 * num_threads)) * param->tid * 6) || ((5 + floor((floor(sqrt(n)) - 5) / (6 * num_threads)) * param->tid * 6) < 5)) ? 5 : (5 + floor((floor(sqrt(n)) - 5) / (6 * num_threads)) * param->tid * 6);
    param->max = ((5 + floor((floor(sqrt(n)) - 5) / (6 * num_threads)) * (param->tid + 1) * 6) > sqrt(n)) ? sqrt(n) : (5 + floor((floor(sqrt(n)) - 5) / (6 * num_threads)) * (param->tid + 1) * 6);
    if (param->max < 0) // if max is negative, make it 5
      param->max = sqrt(n);
    if (param->tid == num_threads - 1) // if last thread's max isn't sqrt(n) = max, make it sqrt(n)
      if (param->max != sqrt(n))
        param->max = sqrt(n);

    // synchronize
    pthread_barrier_wait(&barr);

    do_part_two(param, n);
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
  }

  if (queue.empty())
  {
    std::cout << "File read is empty, exiting...\n";
    exit(-1);
  }

  // create threads
  pthread_t threads[nThreads];
  // create struct for threads
  struct Param param[nThreads];

  // create a barrier for total number of threads.
  pthread_barrier_init(&barr, NULL, nThreads);
  // create semaphore
  sem_init(&sem, 0, 0);
  // create mutex
  pthread_mutex_init(&mtx, NULL);
  // create condition variable
  pthread_cond_init(&cond, NULL);

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