// shm_no_sync.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <signal.h>
#include <limits.h>

#define PAGE_SIZE 4096
#define BUF_SIZE (PAGE_SIZE / sizeof(unsigned int))

unsigned int *buf = NULL;
pid_t writer_pid = 0, reader_pid = 0;

void handler(int sig) {
    if (sig == SIGINT) {
        printf("[%d] Получен SIGINT, завершаюсь\n", getpid());
        if (buf) munmap(buf, PAGE_SIZE);
        exit(0);
    }
}

int main() {
    signal(SIGINT, handler);

    buf = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (buf == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        munmap(buf, PAGE_SIZE);
        return 1;
    }
    //writer
    if (pid == 0) {
        writer_pid = getpid();
        unsigned int num = 0;
        printf("[Писатель %d] Запущен\n", getpid());

        while (1) {
            for (int i = 0; i < BUF_SIZE; i++) {
                buf[i] = num++;
            }

        }
    } 
    //reader
    else {
        reader_pid = getpid();
        printf("[Читатель %d] Запущен\n", getpid());

        unsigned int expected = 0;
        unsigned int last = 0;
        int first = 1;

        while (1) {
            for (int i = 0; i < BUF_SIZE; i++) {
                unsigned int val = buf[i];

                if (!first && val != expected) {
                    printf("[Читатель] ОБНАРУЖЕН СБОЙ последовательности! ""Ожидалось: %u, прочитано: %u (последнее было %u)\n",expected, val, last);
                }
                last = val;
                expected = val + 1;
                first = 0;
            }
        }
    }

    return 0;
}