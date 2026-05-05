#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if (argc < 2) {
    fprintf(2, "Error: incorrect usage.\n");
    exit(1);
  }

  int mask = atoi(argv[1]);
  int dur = (argc > 2) ? atoi(argv[2]) : -1;

  logctl(mask, dur);
  if (dur == 0)
    printf("Log mask=%d duration=indefinite\n", mask);
  else if (dur == -1)
    printf("Log mask=%d\n", mask);
  else
    printf("Log mask=%d duration=%d ticks\n", mask, dur);
  exit(0);
}