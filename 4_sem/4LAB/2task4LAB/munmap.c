#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#define PAGE_SIZE 4096
#define NO_FD (-1)

void AllocateAddressRegion() {
    int ret = 0;
    char *region[10];
    printf("Press enter to allocate 10 pages");
    getchar();
    for (int i = 0; i < 10; ++i) {
        region[i] = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, NO_FD, 0);
        if (region[i] == MAP_FAILED) {
            perror("mmap");
            exit(1);
        }
    }
    printf("Press enter to clear 4-6 pages");
    getchar();
    for (int i = 4; i < 7; ++i) {
        if (munmap(region[i], PAGE_SIZE) == -1) {
            perror("munmap");
            exit(1);
        }
    }
    printf("Deallocated address region\n");
    sleep(10);
}

int main(int argc, char **argv) {
    printf("PID: %d\n", getpid());
    sleep(10);
    AllocateAddressRegion();
    return 0;
}