#include "spinlock.h"

void custom_spinlock_init(custom_spinlock_t *spin) {
    spin->lock = 0;
}

int custom_spinlock_lock(custom_spinlock_t *spin) {
    while (!__sync_bool_compare_and_swap(&spin->lock, 0, 1)) { }
    return 0;
}

int custom_spinlock_unlock(custom_spinlock_t *spin) {
    __atomic_store_n(&spin->lock, 0, __ATOMIC_RELEASE);
    return 0;
}

void custom_spinlock_destroy(custom_spinlock_t *spin) {
    (void)spin;
}
