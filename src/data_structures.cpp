#include <stdio.h>
#include <cstring>
#include "types.cpp"

struct String {
    char* data;
    u64 size;

    char& operator[](u64 i) {
        static char zero = 0;
        if (i >= size) {
            perror("Warning: wrote to an out of bounds string index!");
            i = size - 1;
        }
        return data[i];
    }

    char operator[](u64 i) const {
        if (i >= size) {
            perror("Warning: accesses out of bounds string value!");
            return 0;
        }
        return data[i];
    }


    bool operator==(const char* s) const {
        size_t len = std::strlen(s);
        if (size != len) return false;
        return std::memcmp(data, s, size) == 0;
    }
};

