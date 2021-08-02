
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cinttypes>
#include <cmath>
#include <iostream>

int64_t getSmallestDivisor(int64_t n)
{
  if (n <= 3)
    return 0; // 2 and 3 are primes
  if (n % 2 == 0)
    return 2; // handle multiples of 2
  if (n % 3 == 0)
    return 3; // handle multiples of 3
  int64_t i = 5;
  int64_t max = sqrt(n);
  while (i <= max)
  {
    if (n % i == 0)
      return i;
    if (n % (i + 2) == 0)
      return i + 2;
    i += 6;
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
    std::cout << "Warning: Bad arguments. 1 <= nThreads <= 256!\n";
  }
  if (nThreads != 1)
  {
    std::cout << "Error: I am not multithreaded yet :-(\n";
    exit(-1);
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
    int64_t div = getSmallestDivisor(num);
    // the line below may help you with debugging
    std::cout << "Debug: " << num << " --> " << div << "\n";
    sum += div;
  }

  /// report results
  std::cout << "Sum of divisors = " << sum << "\n";

  return 0;
}
