#include <stdio.h>

#define PATH_MAX 255

int main(void) {
  FILE *fp;
  char path[PATH_MAX];

  fp = popen("ls", "r");

  while (fgets(path, PATH_MAX, fp) != NULL)
    printf("%s", path);

  pclose(fp);

  return 0;
}