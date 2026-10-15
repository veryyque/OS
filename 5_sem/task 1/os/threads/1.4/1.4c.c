#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

void cleanup(void *arg) {
    printf("memory freed\n");
    if (arg == NULL) {
        return NULL;
    }
    free(arg);
}

void *mythread(void *arg) {
    char *text;
    pthread_cleanup_push(cleanup, text);
    text = malloc(strlen("hello world") + 1);
    
    if (text == NULL) {
        printf("malloc\n");
        return NULL;
    }
    strcpy(text, "hello world");
    while (1) {
        printf("%s\n", text);
    }
    pthread_cleanup_pop(1);
    return NULL;
}

int main() {
    pthread_t tid;
    void *result;
    if (pthread_create(&tid, NULL, mythread, NULL) != 0) {
        printf("pthread_create failed\n");
        return 1;
    }

    if (pthread_cancel(tid) != 0) {
        printf("pthread_cancel failed\n");
        return 1;
    }

    if (pthread_join(tid, &result) != 0) {
        printf("pthread_join failed\n");
        return 1;
    }

    if (result == PTHREAD_CANCELED) {
        printf("thread was cancelled\n");
    }
    return 0;
}