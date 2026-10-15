#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <sys/types.h>
#include <stddef.h>

typedef struct {
    pid_t tid;          //идентификатор потока в ядре
    void *stack;        //начало выделенного стека
    size_t stack_size;  //размер стека
    int detached;       //0 joinable, 1 detached
    int joined;         //был ли уже join
    int canceled;       //был ли поток отменен
    volatile int ctid;  //используется для ожидания завершения

    void *retval;
} mythread_t;


int mythread_create(mythread_t *thread, void *(*start_routine)(void *), void *arg);

int mythread_join(mythread_t *thread, void **retval);

int mythread_detach(mythread_t *thread);

int mythread_cancel(mythread_t *thread);


#endif