#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void *mythread(void *arg) {
    int counter = 0;
    while (1) {
        counter++;
    }
    return NULL;
}

int main() {
    pthread_t tid;
    pthread_create(&tid, NULL, mythread, NULL);
    sleep(1);
    pthread_cancel(tid);
    pthread_join(tid, NULL);
    printf("worker was cancelled\n");
    return 0;
}