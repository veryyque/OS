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
    if (data == NULL) {
        perror("malloc");
        return 1;
    }

    data->number = 67;
    data->text = "hello";

    if (pthread_attr_init(&attr) != 0) {
        perror("pthread_attr_init");
        free(data);
        return 1;
    }

    if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) != 0) {
        perror("pthread_attr_setdetachstate");
        pthread_attr_destroy(&attr);
        free(data);
        return 1;
    }

    if (pthread_create(&tid, &attr, mythread, data) != 0) {
        perror("pthread_create");
        pthread_attr_destroy(&attr);
        free(data);
        return 1;
    }

    if (pthread_attr_destroy(&attr) != 0) {
        perror("pthread_attr_destroy");
    }

    pthread_exit(NULL);
}