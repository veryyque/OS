#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

#include <sched.h>

#include "queue.h"

#define QUEUE_SIZE 1000
#define READER_CPU 1
#define WRITER_CPU 0
#define VALUES 1000000

#define RED "\033[41m"
#define NOCOLOR "\033[0m"

void set_cpu(int n) {
	int err;
	cpu_set_t cpuset;
	pthread_t tid = pthread_self();

	CPU_ZERO(&cpuset);
	CPU_SET(n, &cpuset);

	err = pthread_setaffinity_np(tid, sizeof(cpu_set_t), &cpuset);
	if (err) {
		fprintf(stderr, "set_cpu: CPU %d: %s\n", n, strerror(err));
		exit(EXIT_FAILURE);
	}

	printf("set_cpu: set cpu %d\n", n);
}

void *reader(void *arg) {
	int expected = 0;
	queue_t *q = (queue_t *)arg;
	printf("reader [%d %d %d]\n", getpid(), getppid(), gettid());

	set_cpu(READER_CPU);

	while (expected < VALUES) {
		int val = -1;
		int ok = queue_get(q, &val);
		if (!ok)
			continue;

		if (expected != val) {
			printf(RED"ERROR: get value is %d but expected - %d" NOCOLOR "\n", val, expected);
			exit(EXIT_FAILURE);
		}

		expected++;
	}

	return NULL;
}

void *writer(void *arg) {
	int i = 0;
	queue_t *q = (queue_t *)arg;
	printf("writer [%d %d %d]\n", getpid(), getppid(), gettid());

	set_cpu(WRITER_CPU);

	while (i < VALUES) {
		int ok = queue_add(q, i);
		if (!ok)
			continue;
		i++;
	}

	return NULL;
}

int main() {
	pthread_t reader_tid, writer_tid;
	queue_t *q;
	int err;

	printf("main [%d %d %d]\n", getpid(), getppid(), gettid());

	printf("Experiment: capacity=%d, reader CPU=%d, writer CPU=%d, yield=yes\n",
		QUEUE_SIZE, READER_CPU, WRITER_CPU);
	q = queue_init(QUEUE_SIZE);

	err = pthread_create(&reader_tid, NULL, reader, q);
	if (err) {
		printf("main: pthread_create() failed: %s\n", strerror(err));
		return -1;
	}

	sched_yield();

	err = pthread_create(&writer_tid, NULL, writer, q);
	if (err) {
		printf("main: pthread_create() failed: %s\n", strerror(err));
		return -1;
	}

	err = pthread_join(writer_tid, NULL);
	if (err) {
		printf("pthread_join failed: %s\n", strerror(err));
		return 1;
	}
	err = pthread_join(reader_tid, NULL);
	if (err) {
		printf("pthread_join failed: %s\n", strerror(err));
		return 1;
	}

	queue_print_stats(q);
	if (q->count != 0 || q->add_count != VALUES || q->get_count != VALUES) {
		printf("ERROR: wrong counts\n");
		queue_destroy(q);
		return 1;
	}
	printf("OK: all %d values received in order\n", VALUES);
	queue_destroy(q);

	return 0;
}
