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

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cinttypes>
#include <cmath>
#include <iostream>

pthread_mutex_t mtx;
std::queue<int64_t> queue;

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
  int64_t tid = (int64_t)arg;
  while (1)
  {
    pthread_mutex_lock(&mtx);
    if (queue.empty())
    {
      pthread_mutex_unlock(&mtx);
      break;
    }
    pthread_mutex_lock(&mtx);
    int64_t n = queue.front();
    queue.pop();
    std::cout << "Thread:" << tid << "  doing:"
              << n << "\n";
    pthread_mutex_unlock(&mtx);
    if (n <= 3)
    {
      sum += 0; // 2 and 3 are primes
      continue;
    }
    if (n % 2 == 0)
    {
      sum += 2; // handle multiples of 2
      continue;
    }
    if (n % 3 == 0)
    {
      sum += 3; // handle multiples of 3
      continue;
    }
    pthread_mutex_lock(&mtx);
    int64_t i = 5;
    int64_t max = sqrt(n);
    pthread_mutex_unlock(&mtx);
    while (i <= max)
    {
      if (n % i == 0)
      {
        sum += i;
        break;
      }
      if (n % (i + 2) == 0)
      {
        sum += i + 2;
        break;
      }
      i += 6;
    }
    pthread_mutex_unlock(&mtx);
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

  // create threads
  pthread_t threads[nThreads];
  for (int64_t i = 0; i < nThreads; i++)
  {
    pthread_create(&threads[i], NULL, getSmallestDivisor, (void *)i);
  }

  std::cout << "\nHey\n";
  // join threads
  for (int i = 0; i < nThreads; i++)
  {
    pthread_join(threads[i], NULL);
  }
  std::cout << "\nCya\n";
  /// report results
  std::cout << "Sum of divisors = " << sum << "\n";

  return 0;
}
