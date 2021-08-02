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

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cinttypes>
#include <cmath>
#include <iostream>
#include <pthread.h>
#include <algorithm>
#include <vector>

std::vector<int64_t> smallest;

struct Param
{
  int64_t input;
  int64_t startNum;
  int64_t endNum;
  int threadIndex;
};

pthread_barrier_t barrier;

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

void *find(void *a)
{

  struct Param *param = ((struct Param *)a);

  int64_t input = param->input;
  int64_t start = param->startNum;
  int64_t end = param->endNum;
  //int64_t max = sqrt(param->endNum);
  int threadIndex = param->threadIndex;

  //printf("thread:%d start:%ld end:%ld\n", threadIndex+1, start, end);

  smallest.clear();
  while (start <= end)
  {
    if (input % start == 0)
    {
      printf("thread %d found:%ld on %ld\n", threadIndex, start, input);
      smallest.push_back(start);
      break;
    }
    if (input % (start + 2) == 0)
    {
      printf("thread %d found:%ld on %ld\n", threadIndex, start + 2, input);
      smallest.push_back(start + 2);
      break;
    }

    start += 6;
  }

  pthread_barrier_wait(&barrier);
  if (!smallest.empty())
  {
    sort(smallest.begin(), smallest.end());
    //printf("smallest:%ld\n", smallest[0]);
  }
  pthread_exit(0);
}

int64_t getSmallestDivisor(int64_t n, int nThreads)
{

  if (n <= 3)
    return 0; // 2 and 3 are primes
  if (n % 2 == 0)
    return 2; // handle multiples of 2
  if (n % 3 == 0)
    return 3; // handle multiples of 3

  int64_t first = 5;
  int64_t max = ceil(sqrt(n));
  //int64_t max = sqrt(n);

  if (n <= 9999 || nThreads == 1)
  {
    while (first <= max)
    {
      if (n % first == 0)
        return first;
      if (n % (first + 2) == 0)
        return first + 2;
      first += 6;
    }
  }

  else
  {

    int64_t diff = max - first;
    int64_t num = diff / (6 * nThreads);

    struct Param param[nThreads];
    pthread_t threads[nThreads];

    pthread_barrier_init(&barrier, NULL, nThreads);

    for (int i = 0; i < nThreads; i++)
    {

      int64_t start = first;
      int64_t end = start + (6 * num);
      if (i == (nThreads - 1))
        end += (max - end);
      else
        first = end;

      param[i].input = n;
      param[i].startNum = start;
      param[i].endNum = end;
      param[i].threadIndex = i;

      //printf("start:%ld end:%ld\n", start, end);

      pthread_create(&threads[i], NULL, find, &param[i]);
    }

    for (int i = 0; i < nThreads; i++)
    {
      pthread_join(threads[i], NULL);
    }

    //return smallest[0];

    if (!smallest.empty())
    {
      sort(smallest.begin(), smallest.end());
      /*
      for (int f = 0; f < smallest.size(); f++)
      {
        printf("samllests: %ld\n", smallest[f]);
      }
      */
      return smallest[0];
    }
  }

  return 0;
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
    std::cout << "Warning: Bad arguments. Give an integer between 1 and 256!\n";
    nThreads = 1;
  }

  std::cout << "Using " << nThreads
            << " thread" << (nThreads == 1 ? "" : "s")
            << ".\n";

  /// sum up the divisors
  int64_t sum = 0;
  while (1)
  {
    int64_t num;
    if (!(std::cin >> num))
      break;
    int64_t div = getSmallestDivisor(num, nThreads);
    // the line below may help you with debugging
    std::cout << "Debug: " << num << " --> " << div << "\n";
    sum += div;
  }

  /// report results
  std::cout << "Sum of divisors = " << sum << "\n";

  return 0;
}
