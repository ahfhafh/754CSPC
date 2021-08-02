// skeleton program for find duplicate files using sha256 digests
//
// - it reads filenames form standard input
// - it then computes sha256 digests for first two files
// - compares the digests
//
// the program uses C++ containers and strings, but otherwise
// uses standard C APIs
// ===================================================================

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <cassert>
#include <string>
#include <vector>

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

  // for debugging purposes print out the filenames
  printf("Read %lu filenames:\n", fnames.size());
  for (const auto &fname : fnames)
    printf("  '%s'\n", fname.c_str());

  if (fnames.size() < 2)
  {
    printf("I could have worked if you gave me 2+ filenames... :(\n");
    return -1;
  }

  // compute the digests for first 2 files
  std::string dig1 = digest(fnames[0]);
  if (dig1.empty())
  {
    printf("Could not get digest for file %s\n", fnames[0].c_str());
    return -1;
  }
  printf("Digest 1: %s\n", dig1.c_str());
  std::string dig2 = digest(fnames[1]);
  if (dig2.empty())
  {
    printf("Could not get digest for file %s\n", fnames[1].c_str());
    return -1;
  }
  printf("Digest 2: %s\n", dig2.c_str());

  // compare digests
  if (dig1 == dig2)
  {
    printf("First two files are (probably) identical.\n");
  }
  else
  {
    printf("First two files are definitely not identical.\n");
  }

  return 0;
}
