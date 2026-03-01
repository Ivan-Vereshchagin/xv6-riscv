#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  struct procinfo info;
  char *errmsg = "Error: getprocinfo failed.\n";
  char *success_msg = "Success: process info retrieved.\n";

  if (getprocinfo(&info) < 0) {
    write(2, errmsg, strlen(errmsg));
    exit(1);
  }

  printf("ID: %d\n", info.pid);
  printf("Parent ID: %d\n", info.ppid);
  printf("Name: %s\n", info.name);
  printf("State: %d\n", info.state);

  write(1, success_msg, strlen(success_msg));
  exit(0);
}