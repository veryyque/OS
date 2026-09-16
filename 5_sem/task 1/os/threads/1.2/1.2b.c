#include <stdio.h>
#include <pthread.h>

int num = 42;

void *mythread(void *arg) {
    printf("worker started\n");
    printf("worker finished\n");
    return &num;
}

int main() {
    pthread_t tid;
    void *result;
    pthread_create(&tid, NULL, mythread, NULL);
    pthread_join(tid, &result);
    printf("%d \n",*(int *)result);
    printf("main thread finished\n");

    return 0;
}