#include <stdio.h>
#include <pthread.h>
#include <string.h>

void *mythread(void *arg) {
    printf("worker started\n");
    printf("worker finished\n");
    return NULL;
}

int main() {
    pthread_t tid;
    int err = pthread_create(&tid, NULL, mythread, NULL);

    if (err != 0) {
        printf("pthread_create failed: %s\n", strerror(err));
        return 1;
    }
    pthread_join(tid, NULL);
    printf("main thread finished\n");

    return 0;
}