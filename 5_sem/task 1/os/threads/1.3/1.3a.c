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
    data.number = 42;
    data.text = "hello";
    pthread_create(&tid, NULL, mythread, &data);
    pthread_join(tid, NULL);
    return 0;
}