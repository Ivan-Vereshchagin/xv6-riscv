CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGET = pipe_linux

all: $(TARGET)

$(TARGET): pipe_linux.c
	$(CC) $(CFLAGS) -o $(TARGET) pipe_linux.c

clean:
	rm -f $(TARGET)

.PHONY: all clean