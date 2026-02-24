#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int pid;
  int wpid;
  int status = 0;
  int variant = 0;
  
  if (argc > 1) {
    if (argv[1][0] == 'b' || argv[1][0] == 'B' || argv[1][0] == '1') variant = 1;
  }
  
  pid = fork();
  
  if (pid < 0) {
    write(2, "Fork error: fail\n", 18);
    exit(1);
  }
  
  if (pid == 0) {
    pause(70);
    exit(1);
  } else {
    printf("Parent ID: %d, child ID: %d\n", getpid(), pid);
    
    if (variant == 0) {
      wpid = wait(&status);
      
      if (wpid < 0) {
        write(2, "Wait error: fail\n", 18);
        exit(1);
      }
      
      printf("Child %d terminated, exit code: %d\n", wpid, status);
      
    } else {
      
      pause(5);

      if (kill(pid) < 0) {
        write(2, "Kill error: fail\n", 18);
        exit(1);
      }
      
      wpid = wait(&status);
      
      if (wpid < 0) {
        write(2, "Wait error: fail\n", 18);
        exit(1);
      }
      
      printf("Child %d terminated, exit code: %d\n", wpid, status);
    }
    exit(0);
  } 
  exit(0);
}