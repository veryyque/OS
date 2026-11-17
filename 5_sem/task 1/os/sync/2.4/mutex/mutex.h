#ifndef MUTEX_H
#define MUTEX_H

typedef struct {
    int lock;
} custom_mutex_t;

void custom_mutex_init(custom_mutex_t *mutex);
int custom_mutex_lock(custom_mutex_t *mutex);
int custom_mutex_unlock(custom_mutex_t *mutex);
void custom_mutex_destroy(custom_mutex_t *mutex);

#endif
