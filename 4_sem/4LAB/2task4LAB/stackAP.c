#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

void AllocateOnStack() {
    char buffer[4096];
    printf("Allocation: %p\n", &buffer[0]);
    sleep(1);
    AllocateOnStack();
}

int main(int argc, char **argv) {
    printf("PID: %d\n", getpid());
    sleep(10);
    AllocateOnStack();
    return 0;
}