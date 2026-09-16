#include <stdio.h>
#include <pthread.h>
#include <string.h>

void *mythread(void *arg) {
    printf("thread id = %lu\n", (unsigned long)pthread_self());
    return NULL;
}

int main() {
    pthread_t tid;
    int err;
    while (1) {
        err = pthread_create(&tid, NULL, mythread, NULL);
        if (err) {
            printf("pthread_create failed: %s\n", strerror(err));
            break;
        }
    }
    return 0;
}