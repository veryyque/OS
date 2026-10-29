#include "mythread.h"
#include <stdio.h>

void *my_function(void *arg) {
    mythread_t *self = arg;
    while (1) {
        mythread_testcancel(self);
    }
    return NULL;
}

int main(void) {
    mythread_t thread;
    void *result;

    if (mythread_create(&thread, my_function, &thread) == -1) {
        perror("create");
        return 1;
    }

    if (mythread_cancel(&thread) == -1) {
        perror("cancel");
        return 1;
    }

    if (mythread_join(&thread, &result) == -1) {
        perror("join");
        return 1;
    }

    if (result != MYTHREAD_CANCELED) {
        printf("Поток не был отменён");
        return 1;
    }

    printf("Поток отменён, join выполнен");

    mythread_t detached_thread;

    if (mythread_create(&detached_thread, my_function, &detached_thread) == -1) {
        perror("create");
        return 1;
    }

    if (mythread_detach(&detached_thread) == -1) {
        perror("detach");
        return 1;
    }

    if (mythread_cancel(&detached_thread) == -1) {
        perror("cancel");
        return 1;
    }

    while (mythread_cleanup(&detached_thread) == -1) {
        if (errno != EBUSY) {
            perror("cleanup");
            return 1;
        }
    }

    printf("Detached-поток завершён, память освобождена\n");
    return 0;
}
