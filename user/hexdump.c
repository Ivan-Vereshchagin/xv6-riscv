#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

static char hex_chars[] = "0123456789ABCDEF";

int
main(int argc, char *argv[])
{
  if (argc < 3) {
    fprintf(2, "Error: incorrect usage.\n");
    exit(1);
  }

  int count = atoi(argv[1]);
  char *file = argv[2];
  
  int fd = open(file, 0);
  if (fd < 0) {
    fprintf(2, "Error: hexdump cannot open %s.\n", file);
    exit(1);
  }

  char *buf = malloc(count);
  if (!buf) {
    fprintf(2, "Error: hexdump out of memory.\n");
    close(fd);
    exit(1);
  }

  int total = 0;

  while (total < count) {
    int n = read(fd, buf + total, count - total);
    if (n < 0) {
      fprintf(2, "Error: hexdump read error.\n");
      free(buf);
      close(fd);
      exit(1);
    }
    if (n == 0) break;
    total += n;
  }

  for (int i = 0; i < total; i++) {
    unsigned char byte = (unsigned char)buf[i];
    printf("%c%c ", hex_chars[(byte >> 4) & 0x0F], hex_chars[byte & 0x0F]);
  }
  printf("\n");

  free(buf);
  close(fd);
  exit(0);
}
