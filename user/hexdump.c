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

  char buf;
  for (int i = 0; i < count; i++) {
    int n = read(fd, &buf, 1);
    if (n != 1) break;
   
    unsigned char byte = buf & 0xFF;
    char high = hex_chars[(byte >> 4) & 0x0F];
    char low = hex_chars[byte & 0x0F];
    
    printf("%c%c ", high, low);
  }
  printf("\n");
  
  close(fd);
  exit(0);
}
