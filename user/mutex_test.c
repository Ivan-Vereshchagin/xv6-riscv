#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define NPROC 2

void
print_args(int argc, char *argv[])
{
  int i, j;
  int pid = getpid();
  
  for (i = 1; i < argc; i++) {
    char *arg = argv[i];
    for (j = 0; arg[j] != '\0'; j++) {
      char c = arg[j];
      printf("%d: arg %d, char '%c'\n", pid, i, c);
    }
  }
}

static void
error_msg(const char *msg)
{
    write(2, msg, strlen(msg));
}

int
main(int argc, char *argv[])
{
  int mutex_fd;
  int pid;
  int i;

  if (argc < 2) {
    error_msg("Error: incorrect usage\n");
    exit(1);
  }

  printf("Тест без синхронизации\n\n");

  for (i = 0; i < NPROC; i++) {
    pid = fork();
    if (pid < 0) {
      error_msg("Error: fork failed\n");
      exit(1);
    }

    if (pid == 0) {
      print_args(argc, argv);
      exit(0);
    }
  }

  for (i = 0; i < NPROC; i++) wait(0);

  printf("\nТест с синхронизацией\n\n");

  mutex_fd = mutex();
  if (mutex_fd < 0) {
    error_msg("Error: mutex failed\n");
    exit(1);
  }

  for (i = 0; i < NPROC; i++) {
    pid = fork();
    if (pid < 0) {
      error_msg("Error: fork failed\n");
      exit(1);
    }

    if (pid == 0) {
      if (mutex_lock(mutex_fd) < 0) {
        error_msg("Error: mutex_lock failed\n");
        exit(1);
      }

      print_args(argc, argv);

      if (mutex_unlock(mutex_fd) < 0) {
        error_msg("Error: mutex_unlock failed\n");
        exit(1);
      }

      exit(0);
    }
  }
  
  for (i = 0; i < NPROC; i++) wait(0);
  
  if (close(mutex_fd) < 0) {
    error_msg("Error: close failed\n");
    exit(1);
  }

  exit(0);
}