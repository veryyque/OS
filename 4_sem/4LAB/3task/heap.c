#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>

#define HEAP_SIZE (1024 * 1024) //1 mb
#define ALIGN8(x) (((x) + 7) & ~7) //выравнивание по 8 байт

typedef struct Block {
    size_t size;
    int free;
    struct Block *next;
    struct Block *prev; //нужно у некст блока смотреть прев
} Block;

void *heap_start = NULL;
Block *free_list = NULL;

void InitializeHeap() {
    heap_start = mmap(NULL, HEAP_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if (heap_start == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
    free_list = (Block *)heap_start; //1 свободный блок в начале выделенной памяти
    free_list->size = HEAP_SIZE - sizeof(Block);
    free_list->free = 1;
    free_list->next = NULL;
    free_list->prev = NULL; 
}

Block *FindFreeBlock(size_t size) {
    Block *curr = free_list;
    while (curr) {
        if (curr->free && curr->size >= size) {
            return curr; //указатель на свободный блок
        }
        curr = curr->next;
    }
    return NULL;
}

void SplitBlock(Block *block, size_t size) {
    if (block->size >= size + sizeof(Block) + 8) {
        Block *new_block = (Block *)((char *)block + sizeof(Block) + size);
        new_block->size = block->size - size - sizeof(Block);
        new_block->free = 1;
        new_block->next = block->next;
        new_block->prev = block;
        
        block->size = size;
        block->next = new_block;
    
        if (new_block->next) {
            new_block->next->prev = new_block;
        }
    }
}

void *MyMalloc(size_t size) {
    if (!heap_start) {
        InitializeHeap();
    }
    size = ALIGN8(size);
    Block *block = FindFreeBlock(size);
    if (!block) {
        return NULL;
    }
    block->free = 0;
    SplitBlock(block, size);
    return (char *)block + sizeof(Block);
}

int is_valid_pointer(void *ptr) {
    if (ptr == NULL) return 0;
    
    //указатель наход в границах кучи
    if (ptr < heap_start || ptr >= (char*)heap_start + HEAP_SIZE) {
        fprintf(stderr, "MyFree: ERROR - pointer %p is outside heap\n", ptr);
        return 0;
    }
    Block *block = (Block *)((char *)ptr - sizeof(Block));
    
    if ((void*)block < heap_start) {
        fprintf(stderr, "MyFree: ERROR - invalid block header\n", ptr);
        return 0;
    }
    
    if (block->next && block->next->prev != block) {
        fprintf(stderr, "MyFree: ERROR - next->prev != block\n", ptr);
        return 0;
    }
    //проверка двойного вызова free
    if (block->free) {
        fprintf(stderr, "MyFree: WARNING - double free at %p\n", ptr);
        return 0;
    }

    if (block->next) {
        if (block->next->prev != block) {
            fprintf(stderr, "MyFree: ERROR - next->prev corruption\n");
            return 0;
        }
    }
    
    if (block->prev) {
        if (block->prev->next != block) {
            fprintf(stderr, "MyFree: ERROR - prev->next corruption\n");
            return 0;
        }
    }
    

    if (block->size < 8 || block->size > HEAP_SIZE - sizeof(Block)) {
        fprintf(stderr, "MyFree: ERROR - invalid size %zu\n", block->size);
        return 0;
    }

    return 1;
}

void MyFree(void *ptr) {
    if (!ptr) {
        printf("MyFree: free(NULL) ignored\n");
        return;
    }
    
    if (!is_valid_pointer(ptr)) {
        fprintf(stderr, "MyFree: REJECTED invalid pointer %p\n", ptr);
        return;
    }
    //освобождаю
    Block *block = (Block *)((char *)ptr - sizeof(Block));
    block->free = 1;
    
    //объединение свободных блоков
    Block *curr = free_list;
    while (curr && curr->next) {
        if (curr->free && curr->next->free) {
            curr->size += sizeof(Block) + curr->next->size;
            curr->next = curr->next->next;
            if (curr->next) {
                curr->next->prev = curr;
            }
        } else {
            curr = curr->next;
        }
    }
    
    printf("MyFree: successfully freed %p\n", ptr);
}

int main() {
    printf("PID: %d\n", getpid());
    printf("Block size: %zu bytes\n", sizeof(Block));
    printf("Press Enter to start\n");
    getchar();
    
    printf("\nNormal allocations\n");
    void *p1 = MyMalloc(100);
    void *p2 = MyMalloc(300);
    void *p3 = MyMalloc(200);
    printf("p1=%p, p2=%p, p3=%p\n", p1, p2, p3);

    printf("Press Enter to start\n");
    getchar();
    
    printf("\nTesting free with valid pointers\n");
    MyFree(p2);
    MyFree(p1);
    MyFree(p3);
    
    printf("\nTesting error: free in middle of block\n");
    void *p4 = MyMalloc(100);
    printf("Allocated p4 = %p\n", p4);
    
    //указатель в середину блока
    void *mid_ptr = (char*)p4 + 50;
    printf("mid_ptr = p4 + 50 = %p\n", mid_ptr);
    
    printf("\nAttempting to free mid_ptr (should be rejected)\n");
    MyFree(mid_ptr);

    void *p5 = MyMalloc(100);
    printf("Allocated p4 = %p\n", p5);
    void *header_ptr = (char*)p5 - sizeof(Block);  //указывает на заголовок
    printf("header_ptr = %p\n", header_ptr);

    printf("\nAttempting to free not a start of user data\n");
    MyFree(header_ptr);
    
    printf("\nChecking original p4 (should be valid):\n");
    MyFree(p4);
    
    printf("\nTest with NULL\n");
    MyFree(NULL);
    
    printf("\nTest with stack variable\n");
    int stack_var = 42;
    MyFree(&stack_var);
    
    munmap(heap_start, HEAP_SIZE);
    return 0;
}