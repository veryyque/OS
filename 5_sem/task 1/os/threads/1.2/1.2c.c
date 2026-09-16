#include <stdio.h>
#include <pthread.h>

void *mythread(void *arg) {
    printf("worker started\n");
    printf("worker finished\n");
    return "hello world";
}

int main() {
    pthread_t tid;
    void *result;
    pthread_create(&tid, NULL, mythread, NULL);
    pthread_join(tid, &result);
    printf("%s \n",(char *)result);
    printf("main thread finished\n");

    return 0;
}