CC = gcc
CFLAGS = -Wall -Wextra

all: program

mythread.o: mythread.c mythread.h
	$(CC) $(CFLAGS) -c mythread.c -o mythread.o

libmythread.a: mythread.o
	ar rcs libmythread.a mythread.o

program: main.c libmythread.a
	$(CC) $(CFLAGS) main.c -L. -lmythread -o program

clean:
	rm -f *.o *.a program