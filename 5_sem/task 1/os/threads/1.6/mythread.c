#define _GNU_SOURCE
#include "mythread.h"

#include <errno.h>
#include <linux/futex.h>
#include <sched.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <unistd.h>

#define STACK_SIZE 1048576

static int error(int code) {
    errno = code;
    return -1;
}

static int thread_start(void *arg) {
    mythread_t *thread = arg;
    mythread_testcancel(thread);
    thread->retval = thread->start_routine(thread->arg);
    atomic_store(&thread->finished, 1);
    return 0;
}

int mythread_create(mythread_t *thread, void *(*start_routine)(void *), void *arg) {
    if (thread == NULL || start_routine == NULL)
        return error(EINVAL);

    thread->tid = 0;
    thread->stack = NULL;
    thread->stack_size = STACK_SIZE;
    thread->detached = 0;
    thread->ctid = -1;
    thread->retval = NULL;
    thread->start_routine = start_routine;
    thread->arg = arg;
    atomic_init(&thread->canceled, 0); 
    atomic_init(&thread->finished, 0);

    void *stack = mmap(NULL, STACK_SIZE, PROT_READ | PROT_WRITE,MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (stack == MAP_FAILED)
        return -1;
    thread->stack = stack;

    int flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND |CLONE_THREAD | CLONE_SYSVSEM |CLONE_PARENT_SETTID | CLONE_CHILD_CLEARTID;

    int tid = clone(thread_start, (char *)stack + STACK_SIZE, flags, thread, &thread->ctid, NULL, &thread->ctid);
    if (tid == -1) {
        int saved_errno = errno;
        munmap(stack, STACK_SIZE);
        thread->stack = NULL;
        return error(saved_errno);
    }
    thread->tid = tid;
    
    return 0;
}

int mythread_join(mythread_t *thread, void **retval) {
    if (thread == NULL || thread->stack == NULL || thread->detached) return error(EINVAL);
    if (thread->tid == (pid_t)syscall(SYS_gettid)) return error(EDEADLK);

    while (1) {
        int tid = __atomic_load_n(&thread->ctid, __ATOMIC_ACQUIRE);
        if (tid == 0) break;

        int result = syscall(SYS_futex, &thread->ctid, FUTEX_WAIT, tid, NULL, NULL, 0);
        if (result == -1 && errno != EAGAIN && errno != EINTR) return -1;
    }

    if (!atomic_load(&thread->finished)) return error(EIO);
    void *result = thread->retval;
    if (munmap(thread->stack, thread->stack_size) == -1) return -1;
    thread->stack = NULL;
    if (retval != NULL)
        *retval = result;
    return 0;
}

int mythread_detach(mythread_t *thread) {
    if (thread == NULL || thread->stack == NULL || thread->detached) return error(EINVAL);
    thread->detached = 1;
    return 0;
}

int mythread_cleanup(mythread_t *thread) {
    if (thread == NULL || thread->stack == NULL || !thread->detached)
        return error(EINVAL);
    if (__atomic_load_n(&thread->ctid, __ATOMIC_ACQUIRE) != 0)
        return error(EBUSY);
    if (munmap(thread->stack, thread->stack_size) == -1)
        return -1;
    thread->stack = NULL;
    return 0;
}

int mythread_cancel(mythread_t *thread) {
    if (thread == NULL || thread->stack == NULL)
        return error(EINVAL);
    if (__atomic_load_n(&thread->ctid, __ATOMIC_ACQUIRE) == 0)
        return error(ESRCH);
    atomic_store(&thread->canceled, 1);
    return 0;
}

void mythread_testcancel(mythread_t *thread) {
    if (atomic_load(&thread->canceled)) {
        thread->retval = MYTHREAD_CANCELED;
        atomic_store(&thread->finished, 1);
        syscall(SYS_exit, 0);
    }
}
