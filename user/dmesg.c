#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define DMESG_BUF_SIZE 4097

int
main(int argc, char *argv[])
{
  static char buf[DMESG_BUF_SIZE];
  
  int n = dmesg(buf, sizeof(buf));
  
  if (n < 0) {
    fprintf(2, "Error: syscall failed\n");
    exit(1);
  }
  
  if (n > 0) write(1, buf, n);
  
  exit(0);
}