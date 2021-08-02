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
#include <stack>
#include <iostream>

long preOrderTravese(long node, const std::vector<long> fat, std::vector<bool> &visited)
{
  std::stack<long> stk;
  long max_length = 0;

  // push node and curr_length to stack
  long sch_node = node;
  stk.push(node);
  long curr_length = 1;
  stk.push(curr_length);

  while (!stk.empty())
  {
    // pop node and curr_length in stack
    curr_length = stk.top();
    stk.pop();
    sch_node = stk.top();
    stk.pop();

    visited[sch_node] = true;
    for (long i = 0; i < (long)fat.size(); i++)
    {
      // find adjacent nodes
      if (fat[i] == sch_node)
      {
        // push adjacent nodes and increment length
        stk.push(i);
        stk.push(curr_length + 1);

        // update max length
        if (max_length < curr_length + 1)
          max_length = curr_length + 1;
      }
    }
  }

  return max_length;
}

void fatsim(const std::vector<long> fat, long &longest_file_blocks, long &unused_blocks)
{
  std::vector<bool> visited;
  long max_length = 0;

  // fill visited list with false
  for (long i = 0; i < (long)fat.size(); i++)
  {
    visited.push_back(false);
  }

  // find all null pointers and add them to queue to find files
  for (long i = 0; i < (long)fat.size(); i++)
  {
    if (fat[i] == -1) // do dfs on all null pointers
    {
      long s = preOrderTravese(i, fat, visited);
      if (s > max_length)
        max_length = s;
    }
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
