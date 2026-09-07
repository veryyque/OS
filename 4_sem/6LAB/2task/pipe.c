// pipe_demo.c — родитель читает, дочерний пишет
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <limits.h>

#define BUF_SIZE 4096

int main() {
    setbuf(stdout, NULL);
    
    int fd[2]; //fd[0] - read end, fd[1] - write end
    
    if (pipe(fd) == -1) {
        perror("pipe");
        return 1;
    }

    pid_t pid = fork();

    if (pid == 0) {
        //writer
        //close(fd[0]);  //закрываю read - только write

        unsigned int counter = 0;
        int round = 0;

        printf("[Писатель %d] Начинаю запись в pipe\n", getpid());

        while (1) {
            for (int i = 0; i < BUF_SIZE; i++) {
                if (write(fd[1], &counter, sizeof(unsigned int)) == -1) {
                    perror("write");
                    close(fd[1]);
                    exit(1);
                }
                counter++;
                if (counter == UINT_MAX) counter = 0;
            }
            round++;
            printf("[Писатель] Раунд %d: отправлено %d чисел\n", round, BUF_SIZE);
        }

        close(fd[1]);
        exit(0);
    }

    //reader
    //close(fd[1]);  //закрываю write - только read !!!! зачем тут close подробно что будет если убрать close

    unsigned int expected = 0;
    unsigned int value;
    int errors = 0;
    int count = 0;

    printf("[Читатель %d] Начинаю чтение из pipe\n", getpid());
    printf("Нажмите Ctrl+C для завершения\n\n");

    while (read(fd[0], &value, sizeof(unsigned int)) > 0) {
        count++;
        //read = 0 конец данных
        if (value != expected) {
            errors++;
            printf("[Читатель] Ошибка! Ждал %u, получил %u (номер %d)\n",
                   expected, value, count);
            expected = value;
        }
        expected++;
        
        if (expected == UINT_MAX) expected = 0;
        
        if (count % 100000 == 0) {
            printf("[Читатель] Прочитано %d чисел, ошибок: %d\n", count, errors);
        }
    }

    close(fd[0]);
    printf("\n[Читатель] Завершён. Всего прочитано: %d, ошибок: %d\n", count, errors);
    
    wait(NULL);
    printf("Ресурсы освобождены\n");

    return 0;
}