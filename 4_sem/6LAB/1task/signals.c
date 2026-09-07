// ipc_sigwait.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <signal.h>

#define PAGE_SIZE 4096
#define BUF_SIZE (PAGE_SIZE / sizeof(unsigned int))

unsigned int *buf;
pid_t writer_pid;
volatile sig_atomic_t running = 1;

void on_int(int s)  { 
    running = 0; 
}
void on_term(int s) { 
    running = 0; 
}

int main() {
    signal(SIGINT, on_int);
    signal(SIGTERM, on_term);

    sigset_t block; //набор сигналов, битовая маска
    sigemptyset(&block);
    sigaddset(&block, SIGUSR1);
    sigaddset(&block, SIGUSR2);
    sigaddset(&block, SIGINT);    
    sigaddset(&block, SIGTERM);
    sigprocmask(SIG_BLOCK, &block, NULL); //блокировка сигналов из набора block

    buf = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    writer_pid = fork();

    if (writer_pid == 0) {
        //writer
        unsigned int count = 0;
        int round = 0;
        int sig;

        while (1) {
            for (int i = 0; i < BUF_SIZE; i++) buf[i] = count++;
            round++;
            printf("[Писатель] Раунд %d готов\n", round);

            kill(getppid(), SIGUSR1);

            sigwait(&block, &sig);
            if (sig == SIGINT || sig == SIGTERM) break;//cmd C
        }

        printf("[Писатель] Завершён, раундов: %d\n", round);
        exit(0);
    }

    //reader
    unsigned int expected = 0;
    int errors = 0;
    int round = 0;
    int sig;

    while (1) {
        sigwait(&block, &sig);
        if (sig == SIGINT || sig == SIGTERM) break;

        round++;
        for (int i = 0; i < BUF_SIZE; i++) {
            if (buf[i] != expected) {
                errors++;
                printf("[Читатель] Ошибка! Ждал %u, получил %u\n", expected, buf[i]);
                expected = buf[i];
            }
            expected++;
        }
        printf("[Читатель] Раунд %d: OK, ошибок: %d\n", round, errors);

        kill(writer_pid, SIGUSR2);
    }

    printf("[Читатель] Завершён, ошибок: %d\n", errors);
    kill(writer_pid, SIGTERM);
    wait(NULL);
    munmap(buf, PAGE_SIZE);
    return 0;
}