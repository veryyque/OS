CC = gcc
CFLAGS = -Wall -Wextra

all: uthread

uthread: main.o uthread.o
	$(CC) main.o uthread.o -o uthread

main.o: main.c uthread.h
	$(CC) $(CFLAGS) -c main.c

uthread.o: uthread.c uthread.h
	$(CC) $(CFLAGS) -c uthread.c

clean:
	rm -f *.o uthread