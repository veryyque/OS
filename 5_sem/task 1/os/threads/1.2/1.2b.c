#include <stdio.h>
#include <pthread.h>
#include <string.h>

int num = 42;

void *mythread(void *arg) {
    printf("worker started\n");
    printf("worker finished\n");
    return &num;
}

int main() {
    pthread_t tid;
    void *result;
    int err = pthread_create(&tid, NULL, mythread, NULL);

    if (err != 0) {
        printf("pthread_create failed: %s\n", strerror(err));
        return 1;
    }
    pthread_join(tid, &result);
    printf("%d \n",*(int *)result);
    printf("main thread finished\n");

    return 0;
}