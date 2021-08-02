/**********************************************
 * Last Name:   Yang
 * First Name:  Jack
 * Student ID:  30062393
 * Course:      CPSC 457
 * Tutorial:    T05
 * Assignment:  5
 *
 * File name: memsim.cpp
 *********************************************/
#include <stdio.h>
#include <string>
#include <vector>
#include <cassert>
#include <utility>
#include <iostream>
#include <math.h>

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

// all you need to do is to fill in the implementation of MemSim class
struct MemSim
{
  // initializes the memory simulator with a page size
  MemSim(long pg_size)
  {
    partition.push_back(std::make_pair(-1, 0));
    page_size = pg_size;
  }
  // frees all blocks with the given tag and merges adjecent free blocks
  void free(long tag)
  {
    for (long i = 0; i < (long)partition.size(); i++)
    {
      // if chunck is occupied and has a matching tag
      if (partition[i].first == tag)
      {
        // mark the chunck free
        partition[i].first = -1;

        // merge any adjacent free chunks behind
        if (partition[i - 1].first == -1)
        {
          long tmp_size = partition[i - 1].second;
          partition.erase(partition.begin() + i - 1);
          i--;
          partition[i].second += tmp_size;
        }

        // merge any adjacent free chunks forward
        if (partition[i + 1].first == -1)
        {
          long tmp_size = partition[i + 1].second;
          partition.erase(partition.begin() + i + 1);
          partition[i].second += tmp_size;
        }
      }
    }
  }

  // allocates a chunk using best-fit
  // if no suitable chunk found, requests new page(s)
  void alloc(long tag, long size)
  {
    // search through the list of chunks from start to end, and find the smallest chunk
    // in case of ties, pick the first
    bool found = false;
    long best_fit = -1;
    long i = 0;
    for (; i < (long)partition.size(); i++)
    {
      if (partition[i].first == -1 && partition[i].second >= size)
      {
        best_fit = i;
        found = true;
        break;
      }
    }

    // find a better fit if there is one
    if (found == true)
      for (; i < (long)partition.size(); i++)
      {
        if (partition[i].first == -1 && partition[i].second >= size && partition[i].second < partition[best_fit].second)
          best_fit = i;
      }
    // if no suitable chunk found:
    // get minimum number of pages from OS
    // add the new emroy at the end of chunk list
    // the last chunk will be the best chunk
    else if (found == false)
    {
      long additional_size_needed = size - partition.back().second;
      long num_of_pages = std::ceil((double)additional_size_needed / (double)page_size);
      partition.back().second += num_of_pages * page_size;
      best_fit = partition.size() - 1;

      pages_requested += num_of_pages;
    }

    // split the best chunk in two
    // mark the first chunk occupid, and store the tag
    // mark the second chunk free
    partition[best_fit].first = tag;
    long remaining_size = partition[best_fit].second - size;
    partition[best_fit].second = size;
    // don't store an empty chunk in the middle
    if (remaining_size != 0)
      partition.insert(partition.begin() + best_fit + 1, std::make_pair(-1, remaining_size));
    // only at the end
    else if (best_fit == (long)partition.size() - 1)
      partition.insert(partition.begin() + best_fit + 1, std::make_pair(-1, remaining_size));
  }
  // returns statistics about the simulation
  // - number of total page requests
  // - the largest available chunk
  void get_report(long &n_req, long &largest)
  {
    // implement this
    n_req = pages_requested;
    largest = 0;

    // find largest chunk
    for (const auto &i : partition)
    {
      if (i.first == -1 && i.second > largest)
      {
        largest = i.second;
      }
    }
  }

  void test()
  {
    for (long i = 0; i < (long)partition.size(); i++)
    {
      std::cout << "{" << partition[i].first << ", " << partition[i].second << "}, ";
    }
    std::cout << "\n";
  }

private:
  long page_size;
  long pages_requested = 0;
  std::vector<std::pair<long, long>> partition;
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
