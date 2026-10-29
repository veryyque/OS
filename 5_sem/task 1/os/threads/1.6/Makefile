CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -O2 -fno-stack-protector

.PHONY: all test clean
all: program

mythread.o: mythread.c mythread.h
	$(CC) $(CFLAGS) -c mythread.c -o mythread.o

libmythread.a: mythread.o
	ar rcs libmythread.a mythread.o

program: main.c libmythread.a
	$(CC) $(CFLAGS) main.c -L. -lmythread -o program

test: program
	./program

clean:
	rm -f *.o *.a program
