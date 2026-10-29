CC = gcc
CFLAGS = -g -O2 -Wall -Wextra
LDLIBS = -pthread

TARGETS = queue-threads queue-example queue-1000 queue-10000 queue-100000 queue-1000000 queue-different-cpu queue-no-yield queue-sleep

.PHONY: all clean
all: $(TARGETS)

$(TARGETS): %: %.c queue.c queue.h
	$(CC) $(CFLAGS) -pthread queue.c $< $(LDLIBS) -o $@

clean:
	rm -f $(TARGETS) *.o
