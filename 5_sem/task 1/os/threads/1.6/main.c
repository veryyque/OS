#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>

#include "mythread.h"


void *my_function(void *arg) {
    char *text = (char *)arg;
    printf("New thread: pid = %d, tid = %d, message = %s\n",getpid(),gettid(),text);
    return NULL;
}


int main(void) {
    mythread_t thread;
    printf("Main thread: pid = %d, tid = %d\n",getpid(),gettid());
    int result = mythread_create(&thread,my_function, "Hello from mythread!");

    if (result != 0) {
        printf("Failed to create thread\n");
        return 1;
    }

    printf("mythread_create returned success, new tid = %d\n", thread.tid);
    sleep(1);
    return 0;
}