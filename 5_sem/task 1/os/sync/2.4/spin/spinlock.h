#ifndef SPINLOCK_H
#define SPINLOCK_H

typedef struct {
    int lock; //0 - свободно, 1 - занято
} custom_spinlock_t;

void custom_spinlock_init(custom_spinlock_t *spin);
int custom_spinlock_lock(custom_spinlock_t *spin);
int custom_spinlock_unlock(custom_spinlock_t *spin);
void custom_spinlock_destroy(custom_spinlock_t *spin);

#endif
