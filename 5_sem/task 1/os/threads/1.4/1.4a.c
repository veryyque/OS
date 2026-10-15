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
    void *result;
    
    if (pthread_create(&tid, NULL, mythread, NULL) != 0) {
        printf("pthread_create failed\n");
        return 1;
    }

    sleep(1);

    if (pthread_cancel(tid) != 0) {
        printf("pthread_cancel failed\n");
        return 1;
    }

    if (pthread_join(tid, &result) != 0) {
        printf("pthread_join failed\n");
        return 1;
    }

    if (result == PTHREAD_CANCELED) {
        printf("worker was cancelled\n");
    }
    return 0;
}