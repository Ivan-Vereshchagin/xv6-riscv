#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if (argc < 2) {
    fprintf(2, "Error: incorrect usage.\n");
    exit(1);
  }
  
  char *file = argv[1];
  
  int fd = open(file, 0);

  if (fd < 0) {
    fprintf(2, "Error: readcounter cannot open %s.\n", file);
    exit(1);
  }
  
  uint64 count;
  int n = read(fd, &count, sizeof(uint64));
  
  if (n != sizeof(uint64)) {
    fprintf(2, "Readcounter error.\n");
    close(fd);
    exit(1);
  }
  
  printf("Counter: %ld\n", count);
  close(fd);
  exit(0);
}