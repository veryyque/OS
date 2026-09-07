#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv) {
    int ret;
    printf("PID: %d\n", getpid());
    sleep(15);
    ret = execv(argv[0], argv);
    if (ret == -1) {
        perror("execv");
        exit(1);
    }
    printf("Hello world\n");
    return 0;
}