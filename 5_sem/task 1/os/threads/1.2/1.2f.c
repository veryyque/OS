#include <stdio.h>
#include <pthread.h>
#include <string.h>

void *mythread(void *arg) {
    printf("thread id = %lu\n", (unsigned long)pthread_self());
    return NULL;
}

int main() {
    pthread_t tid;
    pthread_attr_t attr;
    int err;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    while (1) {
        err = pthread_create(&tid, &attr, mythread, NULL);
        if (err) {
            printf("pthread_create failed: %s\n", strerror(err));
            break;
        }
    }
    return 0;
}