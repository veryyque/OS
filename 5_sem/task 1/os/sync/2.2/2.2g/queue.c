#define _GNU_SOURCE
#include <pthread.h>
#include <assert.h>

#include "queue.h"

void wait_sem(sem_t *sem) {
	int err;
	do {
		err = sem_wait(sem);
	} while (err == -1 && errno == EINTR);
	if (err == -1) {
		perror("sem_wait");
		abort();
	}
}

void *qmonitor(void *arg) {
	queue_t *q = (queue_t *)arg;

	printf("qmonitor: [%d %d %d]\n", getpid(), getppid(), gettid());

	while (1) {
		queue_print_stats(q);
		sleep(1);
	}

	return NULL;
}

queue_t* queue_init(int max_count) {
	int err;

	queue_t *q = malloc(sizeof(queue_t));
	if (!q) {
		printf("Cannot allocate memory for a queue\n");
		abort();
	}

	q->first = NULL;
	q->last = NULL;
	q->max_count = max_count;
	q->count = 0;

	q->add_attempts = q->get_attempts = 0;
	q->add_count = q->get_count = 0;

	if (sem_init(&q->lock, 0, 1) == -1 ||
		sem_init(&q->empty, 0, max_count) == -1 ||
		sem_init(&q->full, 0, 0) == -1) {
		perror("sem_init");
		abort();
	}

	err = pthread_create(&q->qmonitor_tid, NULL, qmonitor, q);
	if (err) {
		printf("queue_init: pthread_create() failed: %s\n", strerror(err));
		abort();
	}

	return q;
}

void queue_destroy(queue_t *q) {
	if (q == NULL)
        return;

    int err = pthread_cancel(q->qmonitor_tid);
    if (err != 0) {
        fprintf(stderr, "pthread_cancel: %s\n", strerror(err));
        abort();
    }

    err = pthread_join(q->qmonitor_tid, NULL);
    if (err != 0) {
        fprintf(stderr, "pthread_join: %s\n", strerror(err));
        abort();
    }

    qnode_t *node = q->first;

    while (node != NULL) {
        qnode_t *next = node->next;
        free(node);
        node = next;
    }

    sem_destroy(&q->lock);
    sem_destroy(&q->empty);
    sem_destroy(&q->full);
    free(q);
}

int queue_add(queue_t *q, int val) {
	wait_sem(&q->lock);
	q->add_attempts++;
	sem_post(&q->lock);
	wait_sem(&q->empty);
	wait_sem(&q->lock);

	assert(q->count <= q->max_count);

	assert(q->count < q->max_count);

	qnode_t *new = malloc(sizeof(qnode_t));
	if (!new) {
		printf("Cannot allocate memory for new node\n");
		abort();
	}

	new->val = val;
	new->next = NULL;

	if (!q->first)
		q->first = q->last = new;
	else {
		q->last->next = new;
		q->last = q->last->next;
	}

	q->count++;
	q->add_count++;

	sem_post(&q->lock);
	sem_post(&q->full);
	return 1;
}

int queue_get(queue_t *q, int *val) {
	wait_sem(&q->lock);
	q->get_attempts++;
	sem_post(&q->lock);
	wait_sem(&q->full);
	wait_sem(&q->lock);

	assert(q->count >= 0);

	assert(q->count > 0);

	qnode_t *tmp = q->first;

	*val = tmp->val;
	q->first = q->first->next;
	if (q->first == NULL)
		q->last = NULL;

	free(tmp);
	q->count--;
	q->get_count++;

	sem_post(&q->lock);
	sem_post(&q->empty);
	return 1;
}

void queue_print_stats(queue_t *q) {
	wait_sem(&q->lock);
	int count = q->count;
	long add_attempts = q->add_attempts;
	long get_attempts = q->get_attempts;
	long add_count = q->add_count;
	long get_count = q->get_count;
	sem_post(&q->lock);

	printf("queue stats: current size %d; attempts: (%ld %ld %ld); counts (%ld %ld %ld)\n",
		count,
		add_attempts, get_attempts, add_attempts - get_attempts,
		add_count, get_count, add_count - get_count);
}
