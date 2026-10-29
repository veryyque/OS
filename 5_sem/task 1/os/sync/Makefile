CC = gcc
CFLAGS = -g -Wall
LDLIBS = -pthread

EXPERIMENTS = queue-1000 queue-10000 queue-100000 queue-1000000 \
              queue-different-cpu queue-no-yield
TARGETS = queue-example queue-threads $(EXPERIMENTS)

.PHONY: all experiments clean
all: $(TARGETS)

experiments: $(EXPERIMENTS)

$(TARGETS): %: %.c queue.c queue.h
	$(CC) $(CFLAGS) -pthread -I. queue.c $< $(LDLIBS) -o $@

clean:
	rm -f *.o $(TARGETS)
