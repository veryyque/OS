#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ALLOCATIONS 100

void AllocateOnHeap() {
    char *buffers[ALLOCATIONS]; //массив указателей
    for (int i = 0; i < ALLOCATIONS; i++) {
        buffers[i] = malloc(4096);
        if (buffers[i] == NULL) {
            perror("malloc");
            exit(1);
        }
        printf("Allocated %d: %p\n", i + 1, (void*)buffers[i]);
        sleep(1); 
    }
    
    printf("\nPress Enter to free\n");
    getchar();
    
    for (int i = 0; i < ALLOCATIONS; i++) {
        free(buffers[i]);
        printf("Freed %d: %p\n", i + 1, (void*)buffers[i]);
        buffers[i] = NULL;
        sleep(1);
    }
    
    printf("\nAll memory freed\n");
}

int main(int argc, char **argv) {
    printf("PID: %d\n", getpid());
    printf("Press Enter to start allocations\n");
    getchar();
    
    AllocateOnHeap();
    
    printf("\nPress Enter to exit\n");
    getchar();
    return 0;
}