/**********************************************
 * Last Name:   Peng
 * First Name:  Erica
 * Student ID:  30068495
 * Course:      CPSC 457
 * Tutorial:    04
 * Assignment:  Assignment 5 Question 1
 *
 * File name: memsim.cpp
 *********************************************/

#include <stdio.h>
#include <string>
#include <vector>
#include <cassert>
#include <cerrno>
#include <iostream>
#include <list>
#include <set>
#include <vector>
#include <unordered_map>
#include <utility>
#include <algorithm>

typedef std::vector<std::string> vs_t;

// split string p_line into a vector of strings (words)
// the delimiters are 1 or more whitespaces
vs_t split(const std::string &p_line)
{
  auto line = p_line + " ";
  vs_t res;
  bool in_str = false;
  std::string curr_word = "";
  for (auto c : line)
  {
    if (isspace(c))
    {
      if (in_str)
        res.push_back(curr_word);
      in_str = false;
      curr_word = "";
    }
    else
    {
      curr_word.push_back(c);
      in_str = true;
    }
  }
  return res;
}

// convert string to long
// if successful, success = True, otherwise success = False
long str2long(const std::string &s, bool &success)
{
  char *end = 0;
  errno = 0;
  long res = strtol(s.c_str(), &end, 10);
  if (*end != 0 || errno != 0)
  {
    success = false;
    return -1;
  }
  success = true;
  return res;
}

// read in a line from stdin and parse it
// parsed results are passed back via parameter references
// returns true on EOF, otherwise false
// aborts program on bad input
bool parse_line(long &n1, long &n2)
{
  char buff[4096];
  vs_t words;
  static long line_no = 0;

  // read in a line and parse it into words
  while (1)
  { // loop to skip empty lines
    line_no++;
    if (!fgets(buff, sizeof(buff), stdin))
      return true; // eof
    words = split(buff);
    if (words.size() != 0)
      break; // continue if line not empty
  }

  if (words.size() > 2)
  {
    printf("Error on line %ld - too many words\n", line_no);
    exit(-1);
  }

  // convert first word into number
  bool ok;
  n1 = str2long(words[0], ok);
  if (!ok)
  {
    printf("Error on line %ld - not an integer\n", line_no);
    exit(-1);
  }

  if (n1 < 0)
  {
    if (words.size() != 1)
    {
      printf("Error on line %ld - too many numbers\n", line_no);
      exit(-1);
    }
  }
  else
  {
    // convert 2nd number
    n2 = str2long(words[1].c_str(), ok);
    if (!ok || n2 < 1)
    {
      printf("Error on line %ld - 2nd paramter not positive integer\n",
             line_no);
      exit(-1);
    }
  }
  return false;
};

struct MemSim
{

  // vector of pairs of (tag, size)
  std::vector<std::pair<long, long>> chunks;

  long requests = 0;
  long largest_free = 0;
  long min_page;

  // initializes the memory simulator with a page size
  MemSim(long page_size)
  {

    chunks.push_back(std::make_pair(-1, 0));
    min_page = page_size;
  }

  // frees all blocks with the given tag and merges adjecent free blocks
  void free(long tag)
  {

    bool adjacent_free = false;

    // for every chunk
    for (int i = 0; i < chunks.size(); i++)
    {

      // if chunk is occupied and has a matching tag
      // mark the chunk free
      if (chunks[i].first == tag)
      {
        chunks[i].first = -1;
      }
    }

    // determine if there are any adjacent free chunks
    for (int i = 0; i < chunks.size(); i++)
    {
      if (chunks[i].first == -1 && chunks[i + 1].first == -1)
      {
        adjacent_free = true;
        break;
      }
    }

    // while adjacent free chunks exist
    while (adjacent_free)
    {
      // merge any adjacent free chunks
      for (int i = 0; i < chunks.size(); i++)
      {
        if (chunks[i].first == -1 && chunks[i + 1].first == -1)
        {
          long total_memory = chunks[i].second + chunks[i + 1].second;
          chunks[i + 1].second = total_memory;
          chunks.erase(chunks.begin() + i);
          break;
        }
      }

      adjacent_free = false;

      // check again
      for (int i = 0; i < chunks.size(); i++)
      {
        if (chunks[i].first == -1 && chunks[i + 1].first == -1)
        {
          adjacent_free = true;
        }
      }
    }
  }

  // allocates a chunk using best-fit
  // if no suitable chunk found, requests new page(s)
  void alloc(long tag, long size)
  {

    std::vector<long> free_sizes;

    bool no_free = true;
    int best;

    // find all the free memeory and store in a vector
    for (const auto &a : chunks)
    {
      if (a.first != -1)
      {
        free_sizes.push_back(a.second);
      }
    }

    // if free_sizes is not empty
    if (!free_sizes.empty())
    {
      // sort free_sizes in ascending order and get the largest free chunk
      sort(free_sizes.begin(), free_sizes.end());
      largest_free = free_sizes.back();
    }

    // search through the list of free chunks to
    // find smallest chunk that fits requested size
    for (const auto &b : free_sizes)
    {

      // if a chunk is found, set no_free to false
      // and set best to that chunk
      if (b > size)
      {
        no_free = false;
        best = b;
        break;
      }
    }

    // if no suitable chunk found
    if (no_free)
    {
      // get minimum number of pages from OS
      // and add the new memory at the end of chunks
      chunks.push_back(std::make_pair(-1, min_page));
      long new_free = chunks[chunks.size() - 1].second;
      free_sizes.push_back(new_free);

      //the last chunk will be the best chunk
      best = new_free;

      // increment requests
      requests++;
    }

    for (int i = 0; i < chunks.size(); i++)
    {
      if (chunks[i].second == best)
      {
        // split the best chunk in two
        // and mark the second chunk free
        chunks.insert(chunks.begin() + i, std::make_pair(-1, chunks[i].second - size));

        // mark the first chunk occupied, and store the tag in it
        chunks[i].first = tag;
        chunks[i].second = size;
        break;
      }
    }

    // if free_sizes is not empty
    if (!free_sizes.empty())
    {
      // sort free_sizes in ascending order and get the largest free chunk
      sort(free_sizes.begin(), free_sizes.end());
      largest_free = free_sizes.back();
    }
  }

  // returns statistics about the simulation
  // number of total page requests
  // the largest available chunk
  void get_report(long &n_req, long &largest)
  {

    for (int i = 0; i < chunks.size(); i++)
    {
      std::cout << "{" << chunks[i].first << ", " << chunks[i].second << "}";
    }

    std::cout << std::endl;

    n_req = requests;
    largest = largest_free;
  }

private:
};

int main(int argc, char **argv)
{
  // parse command line arguments
  // ------------------------------
  long page_size = 1000;
  if (argc != 2)
  {
    printf("Assuming page size = %ld.\n", page_size);
  }
  else
  {
    bool ok;
    page_size = str2long(argv[1], ok);
    if (!ok || page_size < 1)
    {
      printf("Bad page size '%s'.\n", argv[1]);
      exit(-1);
    }
  }

  // process every line
  // ------------------------------
  MemSim ms(page_size);
  while (true)
  {
    long n1, n2;
    if (parse_line(n1, n2))
      break;
    if (n1 < 0)
      ms.free(-n1);
    else
      ms.alloc(n1, n2);
  }

  // report results
  // ------------------------------
  long n_page_requests, largest_free_chunk_size;
  ms.get_report(n_page_requests, largest_free_chunk_size);
  printf("pages requested: %ld\n", n_page_requests);
  printf("largest free chunk: %ld\n", largest_free_chunk_size);

  return 0;
}
