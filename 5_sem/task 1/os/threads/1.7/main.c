#include "uthread.h"

#include <stdio.h>


void *my_function(void *arg) {
    char *message = (char *)arg;
    printf("User thread: %s\n", message);
    return NULL;
}


int main() {
    uthread_t thread;
    printf("Main started\n");

    if (uthread_create(&thread, my_function, "Hello from uthread!") != 0) {
        printf("uthread_create error\n");
        return 1;
    }

    printf("User thread created\n");
    if (uthread_run(&thread) != 0) {
        printf("uthread_run error\n");
        return 1;
    }

    printf("Main finished\n");
    return 0;
}