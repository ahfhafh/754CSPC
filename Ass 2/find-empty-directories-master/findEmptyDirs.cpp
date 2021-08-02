/**********************************************
 * Last Name:   Yang
 * First Name:  Jack
 * Student ID:  30062393
 * Course:      CPSC 457
 * Tutorial:    T05
 * Assignment:  2
 *
 * File name: findEmptyDirs.cpp
 *********************************************/

#include <stdio.h>
#include <string>
#include <vector>
#include <sys/types.h>
#include <dirent.h>

int main()
{
  std::vector<std::string> stack;
  stack.push_back(".");
  while (!stack.empty())
  {
    auto dirname = stack.back();
    stack.pop_back();

    DIR *dir = opendir(dirname.c_str());
    int count = 0;
    if (dir)
    {
      while (1)
      {
        // read next item in entry in the directory stream
        dirent *de = readdir(dir);
        // if no other item in directory stream
        if (!de)
        {
          // if count is == 2 then directoy is empty
          if (count == 2)
            printf("%s\n", dirname.c_str());
          break;
        }
        std::string name = de->d_name;
        // increment to count number of items in directory
        count++;
        if (name == "." || name == "..")
          continue;
        std::string path = dirname + "/" + de->d_name;
        stack.push_back(path);
      }
      closedir(dir);
    }
  }

  return 0;
}
