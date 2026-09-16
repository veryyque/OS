#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

struct Data {
    int number;
    char *text;
};

void *mythread(void *arg) {
    struct Data *data = (struct Data *)arg;
    printf("number = %d\n", data->number);
    printf("text = %s\n", data->text);
    free(data);
    return NULL;
}

int main() {
    pthread_t tid;
    pthread_attr_t attr;
    struct Data *data = malloc(sizeof(struct Data));
    data->number = 42;
    data->text = "hello";

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&tid, &attr, mythread, data);
    pthread_attr_destroy(&attr);

    pthread_exit(NULL);
}