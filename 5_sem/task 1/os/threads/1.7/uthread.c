#include "uthread.h"

#include <stdio.h>
#include <stdlib.h>


struct start_info {
    void *(*start_routine)(void *);
    void *arg;
    uthread_t *thread;
};


static void thread_start(struct start_info *info) {
    void *(*start_routine)(void *) = info->start_routine;
    void *arg = info->arg;
    uthread_t *thread = info->thread;
    free(info);
    start_routine(arg);
    thread->finished = 1;
}


int uthread_create(uthread_t *thread, void *(*start_routine)(void *), void *arg) {
    if (thread == NULL || start_routine == NULL) {
        return -1;
    }
    thread->stack = malloc(STACK_SIZE);
    if (thread->stack == NULL) {
        return -1;
    }
    struct start_info *info = malloc(sizeof(struct start_info));
    if (info == NULL) {
        free(thread->stack);
        return -1;
    }

    info->start_routine = start_routine;
    info->arg = arg;
    info->thread = thread;

    if (getcontext(&thread->context) == -1) {
        free(info);
        free(thread->stack);
        return -1;
    }

    thread->context.uc_stack.ss_sp = thread->stack; //задание стека пользовательского потока
    thread->context.uc_stack.ss_size = STACK_SIZE; 
    thread->context.uc_link = &thread->main_context; //куда перейти функции после завершения
    thread->finished = 0;
    makecontext(&thread->context,(void (*)(void))thread_start,1,info);

    return 0;
}


int uthread_run(uthread_t *thread) {
    if (thread == NULL) {
        return -1;
    }

    if (thread->finished) {
        return 0;
    }
     //при завершении функции потока переходим в мейн, сохраняю состояние мейн потока и перехожу в контекст пользовательского потока
    if (swapcontext(&thread->main_context, &thread->context) == -1) {
    return -1;
}

    free(thread->stack);
    thread->stack = NULL;
    return 0;
}
