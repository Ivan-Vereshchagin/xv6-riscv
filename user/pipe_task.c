#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define BUF_SIZE 128

int
main(int argc, char *argv[])
{
  int pipefd[2];
  int pid;
  int status;
  
  if (pipe(pipefd) < 0) {
    write(2, "Pipe error\n", 11);
    exit(1);
  }
  
  pid = fork();
  
  if (pid < 0) {
    write(2, "Fork error\n", 11);
    exit(1);
  }
  
  if (pid == 0) {
    if (close(pipefd[1]) < 0) {
      write(2, "Close write end error\n", 21);
      exit(1);
    }
    
    if (close(0) < 0) {
      write(2, "Close stdin error\n", 18);
      exit(1);
    }
    if (dup(pipefd[0]) < 0) {
      write(2, "Dup error\n", 10);
      exit(1);
    }
    if (close(pipefd[0]) < 0) {
      write(2, "Close read end error\n", 21);
      exit(1);
    }
    
    char *argv[] = {"wc", 0};
    exec("wc", argv);
    write(2, "Exec error\n", 11);
    exit(1);
    
  } else {
    if (close(pipefd[0]) < 0) {
      write(2, "Close read end error\n", 21);
      exit(1);
    }
    
    int i;
    for (i = 1; i < argc; i++) {
      char *arg = argv[i];
      int len = strlen(arg);
      int pos = 0;
      
      while (pos < len) {
        int chunk = len - pos;
        if (chunk > BUF_SIZE)
          chunk = BUF_SIZE;
        
        int written = 0;
        while (written < chunk) {
          int n = write(pipefd[1], arg + pos + written, chunk - written);
          if (n < 0) {
            write(2, "Write error\n", 12);
            exit(1);
          }
          written += n;
        }
        pos += chunk;
      }
      
      if (write(pipefd[1], "\n", 1) < 0) {
        write(2, "Write error\n", 12);
        exit(1);
      }
    }
    
    if (close(pipefd[1]) < 0) {
      write(2, "Close write end error\n", 21);
      exit(1);
    }
    
    if (wait(&status) < 0) {
      write(2, "Wait error\n", 11);
      exit(1);
    }
    
    exit(0);
  }
}