// pipe_redirect.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int fd[2]; //fd[0] read, fd[1] write
    
    if (pipe(fd) == -1) {
        perror("pipe");
        return 1;
    }

    pid_t pid = fork();

    if (pid == 0) {
        //читаю из pipe через stdin
        
        close(fd[1]); //close write only read
        
        dup2(fd[0], STDIN_FILENO); //stdin на fd[0](read) копия файлового дискриптора
        close(fd[0]);
        
        printf("[Дочерний %d] Читаю из stdin (перенаправлен в pipe):\n", getpid());
        printf("[Дочерний]--------------------------------\n");
        
        char buf[4096];
        while (fgets(buf, sizeof(buf), stdin) != NULL) {
            printf("[Дочерний] Получил: %s", buf);
        }
        
        printf("[Дочерний]-------------------------------\n");
        printf("[Дочерний] stdin закрыт (родитель перестал писать), завершаюсь\n");
        exit(0);
    }
    //пишу в pipe через stdout
    
    close(fd[0]);
    
    dup2(fd[1], STDOUT_FILENO); //stdout на fd[1]
    close(fd[1]);
    
    printf("Привет из родительского процесса!\n");
    printf("Это сообщение ушло в pipe.\n");
    printf("Дочерний процесс прочитает его через свой stdin.\n");
    printf("Родитель PID=%d\n", getpid());
    
    fclose(stdout); //дочерний считает что данных больше нет
    
    wait(NULL);
    
    fprintf(stderr, "[Родитель] Дочерний процесс завершился\n");
    
    return 0;
}