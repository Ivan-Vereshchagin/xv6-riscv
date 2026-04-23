#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{

  if(argc < 4) {
    fprintf(2, "Error: incorrect usage.\n");
    exit(1);
  }

  int major = atoi(argv[2]);
  int minor = atoi(argv[3]);

  if (mknod(argv[1], major, minor) < 0) {
    fprintf(2, "Error: mknod %s failed.\n", argv[1]);
    exit(1);
  }

  exit(0);
}
