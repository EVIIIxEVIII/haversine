#include <stdlib.h>
#include <sys/mman.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>

#include "types.cpp"

#define PAGE_SIZE 4096

struct Arena {
    void* data;
    u64 current;
    u64 size;

    Arena(u64 allocSize) {
        u64 trueAllocSize = (allocSize + PAGE_SIZE - 1) & ~static_cast<u64>(PAGE_SIZE - 1);

        void* memory = mmap(NULL, trueAllocSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (memory == MAP_FAILED) {
            perror("Error: arena initialization failed!\n");
            data = nullptr;
            current = 0;
            size = 0;
            return;
        }

        volatile char* p = static_cast<volatile char*>(memory);
        for (u64 i = 0; i < trueAllocSize; i+=PAGE_SIZE) p[i] = static_cast<char>(i);

        data = memory;
        current = 0;
        size = trueAllocSize;
    }

    void* pageAllignedAlloc(u64 allocSize) {
        u64 ps = static_cast<u64>(sysconf(_SC_PAGESIZE));

        u64 aligned = (current + ps - 1) & ~static_cast<u64>(ps - 1);
        if (size < allocSize + aligned) {
            printf("Warning: arena ran out of memory!\n");
            return nullptr;
        }

        current = aligned;
        void* ptr = static_cast<char*>(data) + current;
        current += allocSize;

        return ptr;
    }

    void* alloc(u64 allocSize) {
        assert((reinterpret_cast<uptr>(data) & 63) == 0);

        u64 aligned = (current + 63) & ~static_cast<u64>(63);
        if (size < aligned + allocSize) {
            printf("Warning: arena ran out of memory!\n");
            return nullptr;
        }

        current = aligned;
        void* ptr = static_cast<char*>(data) + current;
        current += allocSize;

        return ptr;
    }

    ~Arena() {
       munmap(data, size);
    }
};

