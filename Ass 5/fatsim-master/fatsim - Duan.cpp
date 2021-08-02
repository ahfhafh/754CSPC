/**********************************************
 * Last Name:   Le
 * First Name:  Duan
 * Student ID:  30063948
 * Course:      CPSC 457
 * Tutorial:    T02
 * Assignment:  5
 * Question:    2
 *
 * File name: fatsim.cpp
 *********************************************/

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <list>
#include <queue>

// Iterative BFS
// Adapted from: https://www.geeksforgeeks.org/breadth-first-search-or-bfs-for-a-graph/
void bfs(std::vector<std::list<long>> const &adjList, long start, long n, long &used_blocks, long &currentLongest)
{
	std::queue<long> queue;
	queue.push(start);
	long longestPath = 0;

	while (!queue.empty()) 
	{
		long size = queue.size();

		while (size--)
		{
			start = queue.front(); 
			queue.pop(); 
	
			used_blocks++; 

			for (auto i = adjList[start].begin(); i != adjList[start].end(); ++i)
			{ 
				queue.push(*i);
			}
		}

		longestPath++; 
	}

	if (currentLongest < longestPath)
		currentLongest = longestPath;
}

void fatsim(const std::list<long> fat, long &longest_file_blocks, long &unused_blocks, long n)
{
	std::vector<std::list<long>> adjList(n);
	std::list<long> starterNodes;
	long used_blocks = 0;
	long currentLongest = 0;
	
	// creating adjacency list
	long incr = 0;
	for (const auto &i : fat)
	{
		if (i != -1)
			adjList[i].push_back(incr);
		else
			starterNodes.push_back(incr);
		incr++;
	}

	for (const auto &i : starterNodes)
		bfs(adjList, i, n, used_blocks, currentLongest);

	longest_file_blocks = currentLongest;
	unused_blocks = n - used_blocks;
}

int main(int argc, char **argv)
{
	if (argc != 1)
	{
		printf("Thanks for all the command line arguments.\n"
				"But I don't like any of them... Bye.\n");
		exit(-1);
	}

	std::list<long> fat;
	long maxnum = 0;
	while (1)
	{
		long num;
		if (1 != scanf("%ld", & num)) break;
		if (num < -1)
		{
			printf("I don't like your FAT, it's too negative.\n");
			exit(-1);
   		}
		maxnum = std::max(maxnum, num);
		fat.push_back(num);
  	}

	long size = long(fat.size());
  	if (size <= maxnum)
	{
		printf("Some of your FATs are too big.\n");
		exit(-1);
  	}

	long count1, count2;
	fatsim(fat, count1, count2, size);
	printf("blocks in largest file: %ld\n", count1);
	printf("blocks not in any file: %ld\n", count2);

  	return 0;
}
