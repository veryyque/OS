CC = gcc
CFLAGS = -std=c11 -O2 -g -Wall -Wextra -pthread

all: list

list: main.c queue.c queue.h
	$(CC) $(CFLAGS) main.c queue.c -o list

clean:
	rm -f list *.o

.PHONY: all clean
