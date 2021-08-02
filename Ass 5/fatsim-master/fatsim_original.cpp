#include <stdio.h>
#include <stdlib.h>
#include <vector>

void
fatsim(
    const std::vector<long> fat,
    long & longest_file_blocks,
    long & unused_blocks
       )
{
  // reimplement this
  longest_file_blocks = 8;
  unused_blocks = 42;
}

int
main( int argc, char **)
{
  if( argc != 1) {
    printf("Thanks for all the command line arguments.\n"
           "But I don't like any of them... Bye.\n");
    exit(-1);
  }

  std::vector<long> fat;
  long maxnum = 0;
  while ( 1) {
    long num;
    if( 1 != scanf("%ld", & num)) break;
    if( num < -1) {
      printf("I don't like your FAT, it's too negative.\n");
      exit(-1);
    }
    maxnum = std::max( maxnum, num);
    fat.push_back(num);
  }

  if( long(fat.size()) <= maxnum) {
    printf("Some of your FATs are too big.\n");
    exit(-1);
  }

  long count1, count2;
  fatsim( fat, count1, count2);
  printf("blocks in largest file: %ld\n", count1);
  printf("blocks not in any file: %ld\n", count2);

  return 0;
}
