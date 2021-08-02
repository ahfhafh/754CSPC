/// reads in a positive integers in range (2 .. 2^63-1) from standard input
/// for each number on input:
///   if the number is prime, it is ignored
///   determines the smallest factor of the composite number
/// prints out the sum of all smallest factors
///
/// if no composite numbers are found, prints out "0"
///
/// compile with:
///   $ g++ sumFactors.cpp -O3 -o gcd -lm

#include <stdio.h>
#include <vector>
#include <pthread.h>
#include <queue>
#include <algorithm>

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cinttypes>
#include <cmath>
#include <iostream>

// create mutex
pthread_mutex_t mtx;

// create barrier
pthread_barrier_t barr;

// create queue
std::queue<int64_t> queue;

// number of threads
int64_t num_threads;

// store potential results
std::vector<int16_t> results;

/// sum up the divisors
int64_t sum = 0;

// struct for each thread
struct Param
{
  int64_t i;
  int64_t max;
  int64_t tid;
};

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
    int64_t res = pthread_barrier_wait(&barr);
    if (res == PTHREAD_BARRIER_SERIAL_THREAD)
    {
      if (results.size() >= 1)
      {
        sum += *min_element(results.begin(), results.end());
        results.clear();
      }
    }
    pthread_mutex_lock(&mtx);
    if (queue.empty())
    {
      pthread_mutex_unlock(&mtx);
      break;
    }
    pthread_mutex_unlock(&mtx);

    res = pthread_barrier_wait(&barr);
    int64_t n = queue.front();
    pthread_barrier_wait(&barr);
    if (res == PTHREAD_BARRIER_SERIAL_THREAD)
      queue.pop();
    pthread_mutex_lock(&mtx);
    std::cout << "Thread:" << param->tid << "  doing:"
              << n << "\n";
    pthread_mutex_unlock(&mtx);

    if (n <= 3)
    {
      results.push_back(0); // 2 and 3 are primes
      continue;
    }
    if (n % 2 == 0)
    {
      results.push_back(2); // handle multiples of 2
      continue;
    }
    if (n % 3 == 0)
    {
      results.push_back(3); // handle multiples of 3
      continue;
    }

    pthread_mutex_lock(&mtx);
    param->i = (5 + (floor(sqrt(n)) / (6 * num_threads)) * param->tid * 6);
    param->max = (5 + (floor(sqrt(n)) / (6 * num_threads)) * (param->tid + 1) * 6);
    std::cout << "Num: " << n << "     Thread: " << param->tid << "   Lower Bound: " << param->i << "       Upper Bound: " << param->max << std::endl;
    pthread_mutex_unlock(&mtx);

    pthread_barrier_wait(&barr);

    while (param->i <= param->max)
    {
      if (n % param->i == 0)
      {
        results.push_back(param->i);
        break;
      }
      if (n % (param->i + 2) == 0)
      {
        results.push_back(param->i + 2);
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

  // update global variable for number of threads
  num_threads = nThreads;
  // create threads
  pthread_t threads[nThreads];
  // create struct for threads
  struct Param param[nThreads];
  // create a barrier for total number of threads.
  pthread_barrier_init(&barr, NULL, nThreads);

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

  /// report results
  std::cout << "Sum of divisors = " << sum << "\n";

  return 0;
}
