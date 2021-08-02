/**********************************************
 * Last Name:   Yang
 * First Name:  Jack
 * Student ID:  30062393
 * Course:      CPSC 457
 * Tutorial:    T05
 * Assignment:  2
 *
 * File name: findDupFiles.cpp
 *********************************************/

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <cassert>
#include <string>
#include <vector>
#include <algorithm>
#include <utility>

// number of files
int count = 0;

// read line from fp
// returns False on EOF
//         True  on success
bool get_line(std::string &line, FILE *fp = stdin)
{
  line.clear();
  while (1)
  {
    int c = fgetc(fp);
    if (c == '\n')
      return true;
    if (c == EOF)
      return !line.empty();
    line.push_back(c);
  }
}

// returns SHA256 digest of the contents of file fname
// on failure returns empty string
// uses popen() to call sha256sum binary
std::string
digest(const std::string &fname)
{
  std::string cmdline = "sha256sum " + fname + " 2> /dev/null";
  FILE *fp = popen(cmdline.c_str(), "r");
  if (fp == NULL)
    return "";
  std::string output;
  if (!get_line(output, fp))
    return "";
  if (pclose(fp) != 0)
    return "";
  std::string result;
  for (auto c : output)
    if (isspace(c))
      break;
    else
      result.push_back(c);
  return result;
}

// for sorting
bool compare(const std::pair<std::string, std::string> &a, const std::pair<std::string, std::string> &b)
{
  return (a.second.compare(b.second) < 0);
}

// for comparing digests
void compare_digest(const std::vector<std::pair<std::string, std::string>> &pair_list)
{
  if (pair_list[count].second == pair_list[count + 1].second)
  {
    printf("   - %s\n", pair_list[count].first.c_str());
    count++;
    compare_digest(pair_list);
  }
  else if (pair_list[count].second == pair_list[count - 1].second)
  {
    printf("   - %s\n", pair_list[count].first.c_str());
  }
}

int main()
{
  // read a list of filenames from stdin
  std::vector<std::string> fnames;
  std::string line;
  while (1)
  {
    if (!get_line(line))
      break;
    fnames.push_back(line);
  }

  printf("Reading %lu files...\n", fnames.size());

  // for debugging purposes print out the filenames
  /*
  printf("Read %lu filenames:\n", fnames.size());
  for (const auto &fname : fnames)
    printf("  '%s'\n", fname.c_str());
  */

  if (fnames.size() < 2)
  {
    printf("I could have worked if you gave me 2+ filenames... :(\n");
    return -1;
  }

  // compute digest for all files and store in vector pair
  std::vector<std::string> digest_list;
  std::vector<std::pair<std::string, std::string>> pair_list;
  std::vector<std::string> error_list;

  for (int i = 0; i < fnames.size(); i++)
  {
    std::string dig = digest(fnames[i]);
    if (dig.empty())
    {
      error_list.push_back(fnames[i].c_str());
      fnames.erase(fnames.begin() + i);
      i--;
    }
    else
    {
      //printf("%s    %s\n", fnames[i].c_str(), dig.c_str());
      digest_list.push_back(dig.c_str());
    }
  }

  // store fname and digest in vector pair
  for (int i = 0; i < fnames.size(); i++)
    pair_list.push_back(std::make_pair(fnames[i], digest_list[i]));

  // sort vector of digests and name of files
  sort(pair_list.begin(), pair_list.end(), compare);

  /*
  // print vector pair list
  for (int i = 0; i < digest_list.size(); i++)
  {
    printf("%s    %s\n", pair_list[i].first.c_str(), pair_list[i].second.c_str());
  }
  */

  // compare digests
  int dupfilegroup = 1;
  while (count < pair_list.size())
  {
    if (pair_list[count].second == pair_list[count + 1].second)
    {
      printf("Match %d:\n", dupfilegroup);
      compare_digest(pair_list);
      dupfilegroup++;
    }
    else
      // increment to next file index to read
      count++;
  }

  // print files that digests' weren't able to be computed
  if (error_list.size() > 0)
  {
    printf("Could not get digest for file(s):\n");
    for (int i = 0; i < error_list.size(); i++)
      printf("   - %s\n", error_list[i].c_str());
  }

  if (dupfilegroup == 1)
    printf("There are no duplicate files\n");

  return 0;
}
