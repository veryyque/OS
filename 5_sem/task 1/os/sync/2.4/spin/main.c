#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "spinlock.h"

#define THREADS 4
#define REPEATS 1000000

static custom_spinlock_t spin;
static long long counter = 0;

static void check(int error) {
    if (error) {
        fprintf(stderr, "%s\n", strerror(error));
        exit(EXIT_FAILURE);
    }
}

static void *worker(void *arg) {
    (void)arg;
    for (int i = 0; i < REPEATS; i++) {
        custom_spinlock_lock(&spin);
        counter++;
        custom_spinlock_unlock(&spin);
    }
    return NULL;
}

int main(void) {
    pthread_t threads[THREADS];
    custom_spinlock_init(&spin);

    for (int i = 0; i < THREADS; i++)
        check(pthread_create(&threads[i], NULL, worker, NULL));

    for (int i = 0; i < THREADS; i++)
        check(pthread_join(threads[i], NULL));

    custom_spinlock_destroy(&spin);

    long long expected = (long long)THREADS * REPEATS;
    printf("spin: counter=%lld, expected=%lld: %s\n", counter, expected, counter == expected ? "OK" : "ERROR");
    return 0;
}
