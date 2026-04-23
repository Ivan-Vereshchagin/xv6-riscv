// init: The initial user-level program

#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/spinlock.h"
#include "kernel/sleeplock.h"
#include "kernel/fs.h"
#include "kernel/file.h"
#include "user/user.h"
#include "kernel/fcntl.h"
#include "kernel/pseudo.h"

char *argv[] = { "sh", 0 };

int
main(void)
{
  int pid, wpid;

  if(open("console", O_RDWR) < 0){
    mknod("console", CONSOLE, 0);
    open("console", O_RDWR);
  }
  dup(0);  // stdout
  dup(0);  // stderr

  struct stat st;
  
  if (stat("/dev", &st) < 0) mkdir("/dev");
  if (stat("/dev/null", &st) < 0) mknod("/dev/null", PSEUDO_MAJOR, MINOR_NULL);
  if (stat("/dev/zero", &st) < 0) mknod("/dev/zero", PSEUDO_MAJOR, MINOR_ZERO);
  if (stat("/dev/urandom", &st) < 0) mknod("/dev/urandom", PSEUDO_MAJOR, MINOR_URANDOM);
  if (stat("/dev/nullstat", &st) < 0) mknod("/dev/nullstat", PSEUDO_MAJOR, MINOR_NULLSTAT);

  for(;;){
    printf("init: starting sh\n");
    pid = fork();
    if(pid < 0){
      printf("init: fork failed\n");
      exit(1);
    }
    if(pid == 0){
      exec("sh", argv);
      printf("init: exec sh failed\n");
      exit(1);
    }

    for(;;){
      // this call to wait() returns if the shell exits,
      // or if a parentless process exits.
      wpid = wait((int *) 0);
      if(wpid == pid){
        // the shell exited; restart it.
        break;
      } else if(wpid < 0){
        printf("init: wait returned an error\n");
        exit(1);
      } else {
        // it was a parentless process; do nothing.
      }
    }
  }
}
