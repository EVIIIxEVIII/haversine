#include <stdlib.h>
#include <sys/mman.h>
#include <stdio.h>
#include <assert.h>

#include "types.cpp"

#define PAGE_SIZE 4096

struct Arena {
    void* data;
    u64 current;
    u64 size;

    Arena(u64 allocSize) {
        u64 trueAllocSize = (allocSize + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);

        void* memory = mmap(NULL, trueAllocSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (memory == MAP_FAILED) {
            perror("Error: arena initialization failed!\n");
            data = nullptr;
            current = 0;
            size = 0;
            return;
        }

        volatile char* p = (volatile char*)memory;
        for (u64 i = 0; i < trueAllocSize; i+=PAGE_SIZE) p[i] = i;

        data = memory;
        current = 0;
        size = trueAllocSize;
    }

    void* alloc(u64 allocSize) {
        assert(((uptr)data & 63) == 0);

        u64 aligned = (current + 63) & ~63;
        if (size < aligned + allocSize) {
            printf("Warning: arena ran out of memory!\n");
            return nullptr;
        }

        current = aligned;
        void* ptr = (char*)data + current;
        current += allocSize;

        return ptr;
    }

    ~Arena() {
       munmap(data, size);
    }
};

