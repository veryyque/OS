#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void *mythread(void *arg) {
    while (1) {
        printf("worker is running\n");
    }
    return NULL;
}

int main() {
    pthread_t tid;
    pthread_create(&tid, NULL, mythread, NULL);
    pthread_cancel(tid);
    pthread_join(tid, NULL);
    printf("worker was cancelled\n");
    return 0;
}