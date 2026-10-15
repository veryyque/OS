#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

void sigint_handler(int signo) {
    printf("thread 2: SIGINT received\n");
}

void *thread1(void *arg) {
    sigset_t set;
    if (sigfillset(&set) != 0) { //заполнение набора сигналов
        printf("sigfillset failed\n");
        return NULL;
    }

    if (pthread_sigmask(SIG_BLOCK, &set, NULL) != 0) {
        printf("pthread_sigmask failed in thread 1\n");
        return NULL;
    } //замена текущей маски
    printf("thread 1: all signals blocked\n");
    while (1) {
        sleep(1);
    }

    return NULL;
}

void *thread2(void *arg) {
    sigset_t set;
    if (sigfillset(&set) != 0) {
        printf("sigfillset failed\n");
        return NULL;
    }

    if (sigdelset(&set, SIGINT) != 0) {
        printf("sigdelset failed\n");
        return NULL;
    }

    if (pthread_sigmask(SIG_SETMASK, &set, NULL) != 0) {
        printf("pthread_sigmask failed in thread 2\n");
        return NULL;
    }
    printf("thread 2: waiting for SIGINT\n");
    while (1) {
        sleep(1);
    }
    return NULL;
}

void *thread3(void *arg) {
    sigset_t set;
    int sig; //номер сигнала для сигвэйт
    if (sigemptyset(&set) != 0) {
        printf("sigemptyset failed\n");
        return NULL;
    }

    if (sigaddset(&set, SIGQUIT) != 0) {
        printf("sigaddset failed\n");
        return NULL;
    }

    if (pthread_sigmask(SIG_BLOCK, &set, NULL) != 0) {
        printf("pthread_sigmask failed in thread 3\n");
        return NULL;
    } 
    printf("thread 3: waiting for SIGQUIT\n");
    while (1) {
        if (sigwait(&set, &sig) != 0) {
            printf("sigwait failed\n");
            return NULL;
        }
        if (sig == SIGQUIT) {
            printf("thread 3: SIGQUIT received\n");
        }
    }
    return NULL;
}

int main() {
    pthread_t tid1;
    pthread_t tid2;
    pthread_t tid3;

    struct sigaction sa; //настройка обработчика сигналов

    sa.sa_handler = sigint_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sigaction(SIGINT, &sa, NULL); //обработчик сигналов

    sigset_t set;

    if (sigemptyset(&sa.sa_mask) != 0) {
        printf("sigemptyset failed\n");
        return 1;
    }

    if (sigaction(SIGINT, &sa, NULL) != 0) {
        printf("sigaction failed\n");
        return 1;
    }

    if (sigemptyset(&set) != 0) {
        printf("sigemptyset failed\n");
        return 1;
    }

    if (sigaddset(&set, SIGINT) != 0) {
        printf("sigaddset SIGINT failed\n");
        return 1;
    }

    if (sigaddset(&set, SIGQUIT) != 0) {
        printf("sigaddset SIGQUIT failed\n");
        return 1;
    }

    if (pthread_sigmask(SIG_BLOCK, &set, NULL) != 0) {
        printf("pthread_sigmask failed in main\n");
        return 1;
    }

    if (pthread_create(&tid1, NULL, thread1, NULL) != 0) {
        printf("pthread_create thread 1 failed\n");
        return 1;
    }

    if (pthread_create(&tid2, NULL, thread2, NULL) != 0) {
        printf("pthread_create thread 2 failed\n");
        return 1;
    }

    if (pthread_create(&tid3, NULL, thread3, NULL) != 0) {
        printf("pthread_create thread 3 failed\n");
        return 1;
    }

    if (pthread_join(tid1, NULL) != 0) {
        printf("pthread_join thread 1 failed\n");
        return 1;
    }

    if (pthread_join(tid2, NULL) != 0) {
        printf("pthread_join thread 2 failed\n");
        return 1;
    }

    if (pthread_join(tid3, NULL) != 0) {
        printf("pthread_join thread 3 failed\n");
        return 1;
    }

    return 0;
}