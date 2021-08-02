/**********************************************
 * Last Name:   Le
 * First Name:  Duan
 * Student ID:  30063948
 * Course:      CPSC 457
 * Tutorial:    T02
 * Assignment:  3
 * Question:    1
 *
 * File name: sumFactors.cpp
 *********************************************/

// reads in a positive integers in range (2 .. 2^63-1) from standard input
// for each number on input:
//   if the number is prime, it is ignored
//   determines the smallest factor of the composite number
// prints out the sum of all smallest factors
//
// if no composite numbers are found, prints out "0"
//
// compile with: $ g++ sumFactors.cpp -O3 -lpthread -lm

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cinttypes>
#include <cmath>
#include <iostream>
#include <pthread.h>

typedef struct ThreadArgs
{
	int tid;
	int nThreads;
} ThreadArgs;

int64_t num;
int64_t sum = 0;
int64_t smallestDiv = 9223372036854775807;
volatile int divTID = 256;
bool isRead;
bool isParallel;
pthread_mutex_t mutex;
pthread_barrier_t barrier;
int64_t testRange[256][2];

// read from standard input
// Author: Xining Chen
// D2l
bool read()
{
    if (!(std::cin >> num)) 
		return false;
    
	return true; 
}

int64_t getSmallestDivisorFirstCheck()
{
	if (num <= 3) return 0; // 2 and 3 are primes
	if (num % 2 == 0) return 2; // handle multiples of 2
	if (num % 3 == 0) return 3; // handle multiples of 3

	return -1; // use as an indicator to do parallel work
}

int64_t getSmallestDivisorSecondCheckSerial()
{
	int64_t i = 5;
	int64_t max = sqrt(num);
	
	while (i <= max)
	{
		if (num % i == 0) return i;
		if (num % (i + 2) == 0) return i + 2;
		i += 6;
	}
	
	return 0;
}

void getSmallestDivisorSecondCheckParallel(int64_t start, int64_t end, int tid)
{
	int64_t i = start;
	int64_t max = end;

	while (i <= max && tid < divTID)
	{	 
		if (num % i == 0)
		{
			pthread_mutex_lock(&mutex);
			if (i < smallestDiv)
			{
				smallestDiv = i;
				divTID = tid;
			}
			pthread_mutex_unlock(&mutex);
			return;
		}
		
		if (num % (i + 2) == 0)
		{	
			pthread_mutex_lock(&mutex);
			if ((i + 2) < smallestDiv)
			{
				smallestDiv = i + 2;
				divTID = tid;
			}
			pthread_mutex_unlock(&mutex);
			return;
		}
	
		i += 6;
	}
}

void* threadStart(void* arg)
{
	ThreadArgs* threadArg = (ThreadArgs*)arg;
	int tid = threadArg->tid;
	int nThread = threadArg->nThreads;

	while (1)
	{
		// synchronize all threads
		int result = pthread_barrier_wait(&barrier);
		
		// serial work
		if (result == PTHREAD_BARRIER_SERIAL_THREAD)
		{
			while (1)
			{
				// read standard input into global variable num
				isRead = read();

				// check if reading from standard input was successful
				if (!isRead) break;

				// test for multiples of 2 and 3 in serial
				int64_t div = getSmallestDivisorFirstCheck();
				
				if (div != -1)
				{
					sum += div;
					isParallel = false;
				}
				else
				{
					if (num <= 100000 || nThread == 1)
					{
						sum += getSmallestDivisorSecondCheckSerial();
						continue;
					}
						
					isParallel = true;

					// subdivide test range for parallel work
					// Author: Emmanuel Onu
					// https://repl.it/@emmynex2007/A3-interval-Calc
					int64_t start = 5;
					int64_t max = sqrt(num);
					int64_t diff = max - start;
					int64_t num = diff / (6 * nThread);
					
					for (int i = 0; i < nThread; i++)
					{
						int64_t ts = start;
						int64_t te = ts + (6 * num); 
						
						if (i == (nThread - 1)) 
							te += (max - te);
						else 
							start = te;
						
						// save in global array for threads to have access
						testRange[i][0] = ts;
						testRange[i][1] = te;
					}

					break;
				}
			}
		}
		
		// synchronize before starting parallel work
		pthread_barrier_wait(&barrier);   

		// parallel work
		if (isRead)
		{
			if (isParallel)
				getSmallestDivisorSecondCheckParallel(testRange[tid][0], testRange[tid][1], tid);
		}
		else
			break;

		// synchronize before starting to update sum
		result = pthread_barrier_wait(&barrier);

		// updating sum if parallel work ran
		if (result == PTHREAD_BARRIER_SERIAL_THREAD)
		{
			if (smallestDiv < num)
				sum += smallestDiv;
			divTID = 256;
			smallestDiv = 9223372036854775807;
		}
    }

    pthread_exit(0);
}

int main(int argc, char** argv)
{
	/// parse command line arguments
	int nThreads = 1;
	
	if (argc != 1 && argc != 2)
	{
		std::cout << "Usage: " << argv[0] << " [nThreads]\n";
		exit(-1);
	}
	
	if (argc == 2) nThreads = atoi(argv[1]);

	/// handle invalid arguments
	if (nThreads < 1 || nThreads > 256)
	{
		std::cout << "Error: Bad arguments. 1 <= nThreads <= 256!\n";
		exit(-1);
	}

	std::cout << "Using " << nThreads
			  << " thread" << (nThreads == 1 ? "" : "s")
			  << ".\n";

	// mutex initialization
	long status = pthread_mutex_init(&mutex, NULL);
	if (status != 0)
	{
		std::cout << "Error: pthread_mutex_init() returned error code " << status << "\n";
		exit(-1);
	}

	// barrier initialization
	status = pthread_barrier_init(&barrier, NULL, nThreads);
	if (status != 0)
	{
		std::cout << "Error: pthread_barrier_init() returned error code " << status << "\n";
		exit(-1);
	}
	
	// create threads
	pthread_t threads[nThreads];
	ThreadArgs args[nThreads];
	
	for (int i = 0; i < nThreads; i++)
	{
		args[i].nThreads = nThreads;
		args[i].tid = i;
		long status = pthread_create(&threads[i], NULL, threadStart, &args[i]);
		if (status != 0)
		{
			std::cout << "Error: pthread_create() returned error code " << status << "\n";
			exit(-1);
		}
	}

	// join threads
	for (int j = 0; j < nThreads; j++)
	{
		pthread_join(threads[j], NULL);
	}

	// report result
	std::cout << "Sum of divisors = " << sum << "\n";

	// destroy barrier
	pthread_barrier_destroy(&barrier);

	return 0;
}
