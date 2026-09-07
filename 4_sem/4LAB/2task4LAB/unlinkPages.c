#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#define NO_FD (-1)

void AllocateAddressRegion() {
    int ret = 0;
    char *region[10];
    for (int i = 0; i < 10; ++i) {
        region[i] = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, NO_FD, 0);
        if (region[i] == MAP_FAILED) {
            perror("mmap");
            exit(1);
        }
        printf("Created address region: %p\n", &region[i]);
    }
    getchar();

    for (int i = 4; i < 7; ++i) {
        if (munmap(region[i], 4096) == -1) {
            perror("munmap");
            exit(1);
        }
    }
    printf("Deallocated address region\n");
    sleep(10);
    getchar();
}

int main(int argc, char **argv) {
    printf("PID: %d\n", getpid());
    sleep(10);
    AllocateAddressRegion();
    return 0;
}