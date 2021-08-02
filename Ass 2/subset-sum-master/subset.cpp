/**********************************************
 * Last Name:   Yang
 * First Name:  Jack
 * Student ID:  30062393
 * Course:      CPSC 457
 * Tutorial:    T05
 * Assignment:  2
 *
 * File name: subset.cpp
 *********************************************/
// Uses code from TA file: Pthread-multi-arguments

#include <stdio.h>
#include <vector>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <cmath>

// global variables are bad ...
// but they are acceptable for your first mulithreaded program
std::vector<long> a;

// count number of subsets
int count[32];

// struct to keep info of each thread
struct Param
{
  int startIndex;
  int endIndex;
  int threadIndex;
};

// for debugging purposes (if you want to print the combinations)
void print_sol(long comb)
{
  return; // comment this out if you want to see the subsets

  for (size_t i = 0; i < a.size(); i++)
  {
    if (comb & 1)
      printf("%ld ", a[i]);
    comb /= 2;
  }
  printf("\n");
}

// test a combination 'comb'
// the binary representation of comb will be used to decide
// which numbers in a[] will be used to test the sum
bool test(long comb)
{
  long long sum = 0;
  long bits = comb;
  for (size_t i = 0; i < a.size(); i++)
  {
    if (bits & 1) // check lowest bit
      sum += a[i];
    bits >>= 1; // shift bits to the right
  }
  if (sum == 0)
  {
    print_sol(comb);
    return true;
  }
  else
  {
    return false;
  }
}

// test all combinations in range [from, to]
void *test_range(void *argus)
{
  struct Param *param = ((struct Param *)argus);
  int end = (param->endIndex > (long(1) << a.size())) ? (long(1) << a.size()) : param->endIndex;
  for (int i = param->startIndex; i < end; i++)
  {
    if (test(i))
    {
      // increase counter for specific thread
      count[param->threadIndex] += 1;
    }
  }

  pthread_exit(0);
}

int main(int argc, char **argv)
{
  //
  // parse command line arguments
  //
  if (argc > 2)
  {
    printf("Error: expected at most 1 command line argument.\n");
    return -1;
  }
  long nThreads = 1;
  if (argc > 1)
  {
    // convert argv[1] to 'nThreads', report error if failed
    char *end = 0;
    errno = 0;
    nThreads = strtol(argv[1], &end, 10);
    if (*end != 0 || errno != 0)
    {
      printf("Expected integer, but you gave me '%s'\n", argv[1]);
      return -1;
    }

    if (nThreads > 32 || nThreads < 1)
    {
      printf("Error: number of threads should be between 1 and 32");
      return -1;
    }
  }

  //
  // read all integers one by one and put them into the array
  //
  while (1)
  {
    long n;
    if (1 != scanf("%ld", &n))
      break;
    // append
    a.push_back(n);
  }

  // debug message
  printf("Using %ld thread(s) on %lu numbers.\n",
         nThreads, a.size());

  //
  // to convert this into multithreaded code, you should
  // create threads here, and make each thread count different
  // subsets
  //
  // to make the code work without synchronization mechanisms,
  // you should make separate counters for each thread
  //

  // number of equations to calculate 2^n
  float m = (long(1) << a.size());

  // split the # of equations between number of threads
  int workload = ceil(m / nThreads);
  pthread_t threads[nThreads];
  struct Param param[nThreads];

  // create nThreads of threads
  for (int i = 0; i < nThreads; i++)
  {
    param[i].startIndex = i * workload;
    param[i].endIndex = param[i].startIndex + workload;
    param[i].threadIndex = i;

    pthread_create(&threads[i], NULL, test_range, &param[i]);
  }

  // join nThreads of threads
  for (int i = 0; i < nThreads; i++)
  {
    pthread_join(threads[i], NULL);
  }

  // sum of counters
  int total = 0;
  for (int i = 0; i < nThreads; i++)
  {
    total += count[i];
  }

  //
  // once you join the threads, you can simply sum up the counters
  // to get the total count
  //
  printf("Subsets found: %ld\n", total - 1);

  return 0;
}
