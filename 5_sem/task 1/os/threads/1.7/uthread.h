#ifndef UTHREAD_H
#define UTHREAD_H

#include <ucontext.h>

#define STACK_SIZE 1024 * 1024

typedef struct {
    ucontext_t context;
    void *stack;
    int finished;
} uthread_t;

int uthread_create(uthread_t *thread, void *(*start_routine)(void *), void *arg);
int uthread_run(uthread_t *thread);

#endif