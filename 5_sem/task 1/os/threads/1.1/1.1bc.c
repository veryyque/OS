#define _GNU_SOURCE

#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

int global_var = 67;
pthread_t tids[5]; // POSIX ids

void *mythread(void *arg) {
    int number = *(int *)arg; //номер потока
    int local_var = 1;
    static int static_var = 2;
    const int const_var = 3;

    printf("\n Hello from thread %d\n", number);

    printf("pid: %d\n", getpid());
    printf("ppid: %d\n", getppid());
    printf("gettid(): %d\n", gettid()); //linux kernel thread ID

    printf("pthread_self(): %lu\n",(unsigned long)pthread_self()); //POSIX thread ID
    printf("pthread_create: %lu\n",(unsigned long)tids[number]);

    printf("pthread_equal:  %s\n",pthread_equal(pthread_self(), tids[number])? "YES": "NO");

    printf("address local: %p\n", (void *)&local_var);
    printf("address static local: %p\n", (void *)&static_var);
    printf("address const local:  %p\n", (void *)&const_var);
    printf("address global: %p\n", (void *)&global_var);

    return NULL;
}

int main() {
    int err;
    int numbers[5];

    printf("main [%d %d %d]: Hello from main!\n",getpid(),getppid(),gettid());

    for (int i = 0; i < 5; i++) {
        numbers[i] = i;
        err = pthread_create(&tids[i], NULL, mythread, &numbers[i]);

        if (err) {
            printf("pthread_create() failed: %s\n", strerror(err));
            return -1;
        }
    }

    for (int i = 0; i < 5; i++) {
        pthread_join(tids[i], NULL);
    }
    
    return 0;
}