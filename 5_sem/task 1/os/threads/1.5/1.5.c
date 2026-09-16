#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

void sigint_handler(int signo) {
    printf("thread 2: SIGINT received\n");
}

void *thread1(void *arg) {
    sigset_t set;
    sigfillset(&set);
    pthread_sigmask(SIG_BLOCK, &set, NULL);
    printf("thread 1: all signals blocked\n");
    while (1) {
        sleep(1);
    }

    return NULL;
}

void *thread2(void *arg) {
    sigset_t set;
    sigfillset(&set);
    sigdelset(&set, SIGINT);
    pthread_sigmask(SIG_SETMASK, &set, NULL);
    printf("thread 2: waiting for SIGINT\n");
    while (1) {
        sleep(1);
    }
    return NULL;
}

void *thread3(void *arg) {
    sigset_t set;
    int sig;
    sigemptyset(&set);
    sigaddset(&set, SIGQUIT);
    pthread_sigmask(SIG_BLOCK, &set, NULL);
    printf("thread 3: waiting for SIGQUIT\n");
    while (1) {
        sigwait(&set, &sig); //блокировка выполнения потока пока не придет сигнал SIQUIT
        printf("thread 3: SIGQUIT received\n");
    }
    return NULL;
}

int main() {
    pthread_t tid1;
    pthread_t tid2;
    pthread_t tid3;

    struct sigaction sa;

    sa.sa_handler = sigint_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sigaction(SIGINT, &sa, NULL); //обработчик сигналов

    sigset_t set;

    sigemptyset(&set);
    sigaddset(&set, SIGQUIT);
    pthread_sigmask(SIG_BLOCK, &set, NULL);

    pthread_create(&tid1, NULL, thread1, NULL);
    pthread_create(&tid2, NULL, thread2, NULL);
    pthread_create(&tid3, NULL, thread3, NULL);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_join(tid3, NULL);

    return 0;
}