#define _GNU_SOURCE
#include <linux/futex.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "mutex.h"

static long futex(int *address, int operation, int value) {
    return syscall(SYS_futex, address, operation, value, NULL, NULL, 0);
}

void custom_mutex_init(custom_mutex_t *mutex) {
    mutex->lock = 0;
}

int custom_mutex_lock(custom_mutex_t *mutex) {
    while (!__sync_bool_compare_and_swap(&mutex->lock, 0, 1)) {
        long result = futex(&mutex->lock, FUTEX_WAIT, 1);
        if (result == -1 && errno != EAGAIN && errno != EINTR) {
            perror("futex wait");
            exit(EXIT_FAILURE);
        }
    }
    return 0;
}

int custom_mutex_unlock(custom_mutex_t *mutex) {
    __atomic_store_n(&mutex->lock, 0, __ATOMIC_RELEASE);
    if (futex(&mutex->lock, FUTEX_WAKE, 1) == -1) {
        perror("futex wake");
        exit(EXIT_FAILURE);
    }
    return 0;
}

void custom_mutex_destroy(custom_mutex_t *mutex) {
    (void)mutex;
}
