#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

static int
hex_char_to_int(char c)
{
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'A' && c <= 'F') return c - 'A' + 10;
  if (c >= 'a' && c <= 'f') return c - 'a' + 10;
  return -1;
}

int
main(int argc, char *argv[])
{
  if (argc < 3) {
    fprintf(2, "Error: incorrect usage.\n");
    exit(1);
  }

  char *hexdata = argv[1];
  char *file = argv[2];
  
  int len = strlen(hexdata);
  if(len % 2 != 0) {
    fprintf(2, "Write error.\n");
    exit(1);
  }

  char buf[64];
  int bytes_count = len / 2;

  for (int i = 0; i < len; i += 2) {
    int high = hex_char_to_int(hexdata[i]);
    int low = hex_char_to_int(hexdata[i+1]);
    
    if (high < 0 || low < 0) {
      fprintf(2, "Write error.\n");
      exit(1);
    }
    
    buf[i/2] = (char)((high << 4) | low);
  }

  int fd = open(file, 1);
  if (fd < 0) {
    fprintf(2, "Write error.\n");
    exit(1);
  }
  
  int written = write(fd, buf, bytes_count);
  if (written != bytes_count) {
    fprintf(2, "Write error.\n");
    close(fd);
    exit(1);
  }
  
  close(fd);
  exit(0);
}
