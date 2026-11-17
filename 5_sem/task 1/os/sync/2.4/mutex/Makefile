CC = gcc
CFLAGS = -std=c11 -O2 -g -Wall -Wextra -pthread

all: test

test: main.c mutex.c mutex.h
	$(CC) $(CFLAGS) main.c mutex.c -o test

check: test
	./test

clean:
	rm -f test *.o

.PHONY: all check clean
