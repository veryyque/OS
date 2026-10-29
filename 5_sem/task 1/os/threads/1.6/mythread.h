#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <sys/types.h>
#include <stddef.h>
#include <stdatomic.h>

typedef struct {
    pid_t tid;
    void *stack;
    size_t stack_size;
    int detached;
    atomic_int canceled;
    int ctid; //обнуляется ядром при завершении, иначе содержит tid потока       
    atomic_int finished; 
    void *retval;
    void *(*start_routine)(void *);
    void *arg;
} mythread_t;

#define MYTHREAD_CANCELED ((void *)-1) //для возвращения значения при cancel

int mythread_create(mythread_t *thread, void *(*start_routine)(void *), void *arg);
int mythread_join(mythread_t *thread, void **retval);
int mythread_detach(mythread_t *thread);
int mythread_cancel(mythread_t *thread);
void mythread_testcancel(mythread_t *thread);
int mythread_cleanup(mythread_t *thread);

#endif
