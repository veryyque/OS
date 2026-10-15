#define _GNU_SOURCE

#include "mythread.h"

#include <sched.h>
#include <stdlib.h>
#include <sys/mman.h>

#define STACK_SIZE 1048576 //1 MB

struct start_info {
    void *(*start_routine)(void *);
    void *arg;
};

static int thread_start(void *arg) {
    struct start_info *info = (struct start_info *)arg;

    void *(*start_routine)(void *) = info->start_routine;
    void *user_arg = info->arg;
    free(info);
    start_routine(user_arg); //запуск стартовой функции потока
    return 0;
}


int mythread_create(mythread_t *thread,void *(*start_routine)(void *),void *arg) {
    if (thread == NULL || start_routine == NULL) {
        return -1;
    }

    void *stack = mmap(NULL, STACK_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1,0);
    // mmap(адрес, размер, права, фд, смещение)

    if (stack == MAP_FAILED) {
        return -1;
    }

    struct start_info *info = malloc(sizeof(struct start_info));

    if (info == NULL) {
        munmap(stack, STACK_SIZE);
        return -1;
    }

    info->start_routine = start_routine;
    info->arg = arg;
    void *stack_top = (char *)stack + STACK_SIZE;

    int flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD | CLONE_SYSVSEM;

    pid_t tid = clone(thread_start,stack_top,flags,info);


    if (tid == -1) {
        free(info);
        munmap(stack, STACK_SIZE);
        return -1;
    }

    thread->tid = tid;
    thread->stack = stack;
    thread->stack_size = STACK_SIZE;

    return 0;
}