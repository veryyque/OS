#include <stdio.h>
#include <pthread.h>

struct Data {
    int number;
    char *text;
};

void *mythread(void *arg) {
    struct Data *data = (struct Data *)arg;
    printf("number=%d\n", data->number);
    printf("text=%s\n", data->text);
    return NULL;
}

int main() {
    pthread_t tid;
    struct Data data;
    data.number = 67;
    data.text = "hello";
    if (pthread_create(&tid, NULL, mythread, &data) != 0) {
        perror("pthread_create");
        return 1;
    }

    if (pthread_join(tid, NULL) != 0) {
        perror("pthread_join");
        return 1;
    }

    return 0;
}