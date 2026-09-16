#include <stdio.h>
#include <pthread.h>

void *mythread(void *arg) {
    printf("worker started\n");
    printf("worker finished\n");
    return NULL;
}

int main() {
    pthread_t tid;
    pthread_create(&tid, NULL, mythread, NULL);
    pthread_join(tid, NULL);
    printf("main thread finished\n");

    return 0;
}