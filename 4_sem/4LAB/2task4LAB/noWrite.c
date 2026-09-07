#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <signal.h>

#define NO_FD (-1)

void AllocateAddressRegion() {
    int ret = 0;
    char *address_region;
    printf("Press Enter to start allocations\n");
    getchar();
    address_region = mmap(NULL, 10 * 4096, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, NO_FD, 0);
    printf("Created address region: %p\n", &address_region[0]);
    if (address_region == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
    printf("Press Enter change rights\n");
    getchar();

    ret = mprotect(address_region, 10 * 4096, PROT_READ); //разреш только чтение
    if (ret == -1) {
        perror("mprotect");
        exit(1);
    }
    printf("Changed memory protection: PROT_READ\n");
    *address_region = 'a';
    sleep(5);
}

int main(int argc, char **argv) {
    printf("PID: %d\n", getpid());
    sleep(1);
    AllocateAddressRegion();
    sleep(60);
    return 0;
}