#define _GNU_SOURCE
#include <pthread.h>
#include <stdatomic.h>
#include <linux/futex.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

 
_Static_assert(sizeof(atomic_int) == 4, "Need a 32-bit atomic_int");
static _Alignas(4) atomic_int lock_word;
static long counter;
static int use_mutex;
static long repeats = 1000000;

void lock(void) {
    for (;;) {
        int expected = 0;
        if (atomic_compare_exchange_strong_explicit(&lock_word, &expected, 1,
                memory_order_acquire, memory_order_relaxed)) return;
        if (use_mutex) {
            int result = syscall(SYS_futex, &lock_word, FUTEX_WAIT_PRIVATE, 1,
                                 NULL, NULL, 0);
            if (result == -1 && errno != EAGAIN && errno != EINTR) {
                perror("futex wait"); exit(1);
            }
        }
         
    }
}
void unlock(void) {
    atomic_store_explicit(&lock_word, 0, memory_order_release);
    if (use_mutex && syscall(SYS_futex, &lock_word, FUTEX_WAKE_PRIVATE, 1,
                            NULL, NULL, 0) == -1) {
        perror("futex wake"); exit(1);
    }
}
void *worker(void *arg) {
    (void)arg;
    for (long i = 0; i < repeats; i++) {
        lock(); counter++; unlock();
    }
    return NULL;
}
int main(int argc, char **argv) {
    if (argc < 2 || (strcmp(argv[1], "spin") && strcmp(argv[1], "mutex"))) {
        fprintf(stderr, "Usage: %s spin|mutex [threads] [increments per thread]\n", argv[0]);
        return 1;
    }
    use_mutex = !strcmp(argv[1], "mutex");
    int n = argc > 2 ? atoi(argv[2]) : 4;
    if (argc > 3) repeats = atol(argv[3]);
    if (n < 1 || n > 64 || repeats < 1 || repeats > 100000000) return 1;
    pthread_t threads[64];
    for (int i = 0; i < n; i++) {
        int err = pthread_create(&threads[i], NULL, worker, NULL);
        if (err) { fprintf(stderr, "%s\n", strerror(err)); return 1; }
    }
    for (int i = 0; i < n; i++) {
        int err = pthread_join(threads[i], NULL);
        if (err) { fprintf(stderr, "%s\n", strerror(err)); return 1; }
    }
    printf("counter=%ld expected=%ld: %s\n", counter, n * repeats,
           counter == n * repeats ? "OK" : "ERROR");
    return counter == n * repeats ? 0 : 1;
}
