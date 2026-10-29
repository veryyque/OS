CC = gcc
CFLAGS = -g -O2 -Wall -Wextra
LDLIBS = -pthread

TARGETS = queue-threads queue-example

.PHONY: all clean
all: $(TARGETS)

$(TARGETS): %: %.c queue.c queue.h
	$(CC) $(CFLAGS) -pthread queue.c $< $(LDLIBS) -o $@

clean:
	rm -f $(TARGETS) *.o
