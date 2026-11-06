#define _GNU_SOURCE
#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Storage storage;
 
atomic_int stop;
atomic_ulong iterations[3], pairs[3], swaps[3];

void check(int err) {
    if (err) { fprintf(stderr, "%s\n", strerror(err)); exit(1); }
}
Node *new_node(void) {
    Node *n = calloc(1, sizeof(*n));
    if (!n) { perror("calloc"); exit(1); }
    check(pthread_rwlock_init(&n->sync, NULL));
    return n;
}

void *reader(void *arg) {
    int id = *(int *)arg;
    while (!atomic_load(&stop)) {
        unsigned long found = 0;
        Node *a = storage.first;
        check(pthread_rwlock_rdlock(&a->sync));
        while (a->next) {
            Node *b = a->next;  
            check(pthread_rwlock_rdlock(&b->sync));
            if (a != storage.first) {
                size_t x = strlen(a->value), y = strlen(b->value);
                if ((id == 0 && x < y) || (id == 1 && x > y) ||
                    (id == 2 && x == y)) found++;
            }
            check(pthread_rwlock_unlock(&a->sync));
            a = b;  
        }
        check(pthread_rwlock_unlock(&a->sync));
        atomic_store(&pairs[id], found);
        atomic_fetch_add(&iterations[id], 1);
    }
    return NULL;
}

void *writer(void *arg) {
    int id = *(int *)arg;
    unsigned seed = 1234 + id;
    while (!atomic_load(&stop)) {
        Node *prev = storage.first;
        check(pthread_rwlock_wrlock(&prev->sync));
        while (prev->next) {
            Node *a = prev->next;
            check(pthread_rwlock_wrlock(&a->sync));
            Node *b = a->next;
            if (!b) { check(pthread_rwlock_unlock(&a->sync)); break; }
            check(pthread_rwlock_wrlock(&b->sync));
             
            if (rand_r(&seed) % 2) {
                prev->next = b;
                a->next = b->next;
                b->next = a;
                atomic_fetch_add(&swaps[id], 1);
                check(pthread_rwlock_unlock(&prev->sync));
                check(pthread_rwlock_unlock(&a->sync));
                prev = b;  
            } else {
                check(pthread_rwlock_unlock(&prev->sync));
                check(pthread_rwlock_unlock(&b->sync));
                prev = a;
            }
        }
        check(pthread_rwlock_unlock(&prev->sync));
    }
    return NULL;
}

void queue_init(int n) {
    storage.first = new_node();
    Node *tail = storage.first;
    unsigned seed = 42;
    for (int i = 0; i < n; i++) {
        tail->next = new_node(); tail = tail->next;
        int len = 1 + rand_r(&seed) % 99;
        memset(tail->value, 'a' + i % 26, len);
    }
}

int queue_destroy(int expected) {
     
    int count = 0;
    Node *p = storage.first->next;
    while (p && count <= expected) { count++; p = p->next; }
    if (p || count != expected) { fprintf(stderr, "ERROR: broken list\n"); return 1; }
    p = storage.first;
    while (p) { Node *next = p->next; check(pthread_rwlock_destroy(&p->sync)); free(p); p = next; }
    return 0;
}
