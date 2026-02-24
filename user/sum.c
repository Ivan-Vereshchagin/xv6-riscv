#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

#define MAX_BUF 256

int main(int argc, char *argv[]) {
    char buf[MAX_BUF];
    int i = 0;
    int n;
    char c;

    while (i < MAX_BUF - 1) {
        n = read(0, &c, 1);
        if (n < 0) {
      	    write(2, "Reading error\n", 14);
            exit(1);
    	}
        if (n == 0) {
      	    if (i == 0) {
                write(2, "Error: no input\n", 16);
                exit(1);
      	    }
      	    break;
    	}
        if (c == '\n') break;
        buf[i++] = c;
    }
    buf[i] = '\0';

    if (i == 0) {
    	write(2, "Error: no input\n", 16);
    	exit(1);
    }

    printf("|%s|\n", buf);

    char *space = 0;
    for (int j = 0; j < i; j++) {
        if (buf[j] == ' ') {
            space = &buf[j];
            break;
        }
    }

    if (space) {
        *space = '\0';
        char *second = space + 1;

	if (*second < '0' || *second > '9') {
        write(2, "Error: no second number\n", 25);
        exit(1);
    }

        int num1 = atoi(buf);
        int num2 = atoi(second);
        printf("%d\n", num1 + num2);
    } else {
        write(2, "Error: incorrect input (expecting 2 numbers divided by space)\n", 62);
        exit(1);
    }
    exit(0);
}
