#ifndef QUEUE_H
#define QUEUE_H

#include <pthread.h>
#include <stdatomic.h>

typedef struct Node {
    char value[100];
    struct Node *next;
    pthread_spinlock_t sync;
} Node;

typedef struct Storage {
    Node *first;  
} Storage;

extern Storage storage;
extern atomic_int stop;
extern atomic_ulong iterations[3], pairs[3], swaps[3];

void check(int err);
void queue_init(int n);
int queue_destroy(int expected);
void *reader(void *arg);
void *writer(void *arg);

#endif
