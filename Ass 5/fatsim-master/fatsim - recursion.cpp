/**********************************************
 * Last Name:   Yang
 * First Name:  Jack
 * Student ID:  30062393
 * Course:      CPSC 457
 * Tutorial:    T05
 * Assignment:  5
 *
 * File name: fatsim.cpp
 *********************************************/
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <deque>

void dfs(long node, const std::vector<long> fat, std::vector<bool> &visited, long &max_length, long prev_length)
{
  long curr_length = 1;
  // mark as visited
  visited[node] = true;

  for (long i = 0; i < (long)fat.size(); i++)
  {
    if (fat[i] == node)
    {
      curr_length = prev_length + 1;
      dfs(i, fat, visited, max_length, curr_length);
    }

    if (max_length < curr_length)
      max_length = curr_length;
  }
}

void fatsim(const std::vector<long> fat, long &longest_file_blocks, long &unused_blocks)
{
  std::vector<bool> visited;
  std::deque<long> tmp_queue;
  long max_length = 0;
  // find all null pointers and add them to queue to find files
  for (long i = 0; i < (long)fat.size(); i++)
  {
    if (fat[i] == -1)
      tmp_queue.push_back(i);
  }

  // fill visited list with false
  for (long i = 0; i < (long)fat.size(); i++)
  {
    visited.push_back(false);
  }

  // do dfs on all null pointers
  while (!tmp_queue.empty())
  {
    // index j
    long j = tmp_queue.front();
    tmp_queue.pop_front();

    dfs(j, fat, visited, max_length, 1);
  }

  longest_file_blocks = max_length;
  unused_blocks = 0;

  // count unused blocks
  for (long i = 0; i < (long)visited.size(); i++)
  {
    if (visited[i] == false)
      unused_blocks += 1;
  }
}

int main(int argc, char **)
{
  if (argc != 1)
  {
    printf("Thanks for all the command line arguments.\n"
           "But I don't like any of them... Bye.\n");
    exit(-1);
  }

  std::vector<long> fat;
  long maxnum = 0;
  while (1)
  {
    long num;
    if (1 != scanf("%ld", &num))
      break;
    if (num < -1)
    {
      printf("I don't like your FAT, it's too negative.\n");
      exit(-1);
    }
    maxnum = std::max(maxnum, num);
    fat.push_back(num);
  }

  if (long(fat.size()) <= maxnum)
  {
    printf("Some of your FATs are too big.\n");
    exit(-1);
  }

  long count1, count2;
  fatsim(fat, count1, count2);
  printf("blocks in largest file: %ld\n", count1);
  printf("blocks not in any file: %ld\n", count2);

  return 0;
}
