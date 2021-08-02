/**********************************************
 * Last Name:   Yang
 * First Name:  Jack
 * Student ID:  30062393
 * Course:      CPSC 457
 * Tutorial:    T05
 * Assignment:  1
 *
 * File name: simple_wc.cpp
 *********************************************/

#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <cstdint>
#include <cstring>

int main(int, char **)
{
  int fd = STDIN_FILENO; // input will be standard input

  // our boolean mini-state, true = we are in a middle of a word
  bool inside_word = false;
  // setup 3 counters
  uint64_t word_count = 0;
  uint64_t line_count = 0;
  uint64_t char_count = 0;
  // read file character by character and count words
  while (1)
  {
    char c[1048576];
    if (read(fd, &c, 1048576) < 1)
      break; // quit loop on EOF

    // count every character
    char_count += strlen(c);

    // count new lines and words
    char *z;
    int size = 0;

    for (z = c; *z != '\0'; z++)
    {
      if (*z == '\n')
      {
        line_count++;
      }
      if ((*z == ' ') || (*z == '\t') || (*z == '\n') || (*z == '\v') || (*z == '\f') || (*z == '\r'))
      {
        inside_word = false;
      }
      else if (!inside_word)
      {
        inside_word = true;
        word_count++;
      }
    }
  }

  // print results
  printf("%7lu %7lu %7lu\n",
         line_count,
         word_count,
         char_count);

  return 0;
}
