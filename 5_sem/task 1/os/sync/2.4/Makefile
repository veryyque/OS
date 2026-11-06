CC = gcc
CFLAGS = -std=c11 -O2 -g -Wall -Wextra -pthread
all: locks
locks: locks.c
	$(CC) $(CFLAGS) $< -o $@
clean:
	rm -f locks
.PHONY: all clean
