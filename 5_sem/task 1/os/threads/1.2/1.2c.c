#include <stdio.h>
#include <pthread.h>
#include <string.h>

void *mythread(void *arg) {
    printf("worker started\n");
    printf("worker finished\n");
    return "hello world";
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
    printf("%s \n",(char *)result);
    printf("main thread finished\n");

    return 0;
}