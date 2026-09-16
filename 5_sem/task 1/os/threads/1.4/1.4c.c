#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

void cleanup(void *arg) {
    printf("memory freed\n");
    free(arg);
}

void *mythread(void *arg) {
    char *text = malloc(strlen("hello world") + 1);
    strcpy(text, "hello world");
    pthread_cleanup_push(cleanup, text);
    while (1) {
        printf("%s\n", text);
    }
    pthread_cleanup_pop(1);
    return NULL;
}

int main() {
    pthread_t tid;
    pthread_create(&tid, NULL, mythread, NULL);
    pthread_cancel(tid);
    pthread_join(tid, NULL);
    return 0;
}