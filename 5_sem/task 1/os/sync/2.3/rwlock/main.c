#define _GNU_SOURCE
#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv) {
    int n = argc > 1 ? atoi(argv[1]) : 1000;
    int seconds = argc > 2 ? atoi(argv[2]) : 10;
    if (n < 2 || n > 100000 || seconds < 1) {
        fprintf(stderr, "Usage: %s [nodes: 2..100000] [seconds > 0]\n", argv[0]);
        return 1;
    }
    queue_init(n);
    int ids[3] = {0, 1, 2};
    pthread_t readers[3], writers[3];
    for (int i = 0; i < 3; i++) check(pthread_create(&readers[i], NULL, reader, &ids[i]));
    for (int i = 0; i < 3; i++) check(pthread_create(&writers[i], NULL, writer, &ids[i]));
    for (int t = 0; t < seconds; t++) {
        sleep(1);
        printf("iterations(up/down/equal): %lu %lu %lu; last pairs: %lu %lu %lu; swaps: %lu %lu %lu\n",
            atomic_load(&iterations[0]), atomic_load(&iterations[1]), atomic_load(&iterations[2]),
            atomic_load(&pairs[0]), atomic_load(&pairs[1]), atomic_load(&pairs[2]),
            atomic_load(&swaps[0]), atomic_load(&swaps[1]), atomic_load(&swaps[2]));
    }
    atomic_store(&stop, 1);
    for (int i = 0; i < 3; i++) {
        check(pthread_join(readers[i], NULL)); check(pthread_join(writers[i], NULL));
    }
    if (queue_destroy(n)) return 1;
    printf("OK: %d nodes remain\n", n);
    return 0;
}
