#include <fstream>
#include <vector>
#include <iostream>
#include <cstring>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <charconv>

#include "simple_profiler.cpp"
#include "memory_arena.cpp"
#include "data_structures.cpp"

#define KB(val) val * 1024
#define MB(val) val * 1024 * 1024
#define GB(val) val * 1024 * 1024 * 1024

typedef uint8_t b8;

struct Pair {
    f64 X0, Y0, X1, Y1;
};

struct Answers {
    std::vector<f64> data;
    f64              haversineSum;
};

enum Fields {
    X0, Y0, X1, Y1,
};

struct Pairs {
    Pair* data;
    u64 size;
};

std::vector<char> readFileAsBin(std::string path) {
    struct stat Stat;
    stat(path.data(), &Stat);

    TimeThroughput("readFileAsBin", static_cast<u64>(Stat.st_size));
    std::ifstream file(path, std::ios::binary | std::ios::ate);

    if (!file.is_open()) {
        std::cout << "Failed to open answers file! \n";
        return {};
    }

    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(static_cast<size_t>(fileSize));

    if (!file.read(buffer.data(), fileSize)) {
        std::cout << "Failed to read file into memory! \n";
        return {};
    }

    return buffer;
}

Answers readAnswers(std::string path) {
    TimeFunction();

    std::vector<char> buffer = readFileAsBin(path);
    if(buffer.empty()) return {};
    const char* data = buffer.data();

    const size_t size = *reinterpret_cast<const size_t*>(data);
    data += sizeof(size_t);

    std::vector<f64> answers(size);

    std::memcpy(answers.data(), data, size * sizeof(f64));

    f64 haversineSum = answers.back();
    answers.pop_back();

    return Answers {
        answers,
        haversineSum
    };
}

b8 mapFileToString(i32 fd, String& dest, u64 offset, u64 blockSize) {
    TimeThroughput("mapFileToString", blockSize);

    if(blockSize == 0) return false;
    u64 pageSize = static_cast<u64>(sysconf(_SC_PAGESIZE));

    u64 alignedOffset = static_cast<u64>(offset & ~(pageSize - 1));
    size_t lead = offset - alignedOffset;
    if(lead != 0) return false;

    if(reinterpret_cast<uintptr_t>(dest.data) % pageSize) return false;

    size_t alignedBlockSize = (blockSize + pageSize - 1) & ~(pageSize - 1);

    i32 prot = PROT_READ | PROT_WRITE;
    i32 flags = MAP_PRIVATE | MAP_FIXED;

    void* p = mmap(dest.data, alignedBlockSize, prot, flags, fd, static_cast<off_t>(alignedOffset));
    if (p == MAP_FAILED) return false;

    dest.size = alignedBlockSize;

    return true;
}

Pairs parsePoints(const std::string& path, Arena& targetArena) {
    struct stat Stat;
    stat(path.data(), &Stat);

    u64 MB_100 = MB(100);
    u64 MB_16 = MB(16);

    Arena arena(MB_100);
    if(!arena.data) {
        printf("Failed to allocated parsed arena!\n");
        return { nullptr, 0 };
    }

    Pair* parsedPairs = static_cast<Pair*>(targetArena.alloc(sizeof(Pair) * 10001000));

    String field;
    field.data = static_cast<char*>(arena.alloc(256*sizeof(char)));
    field.size = 0;

    String value;
    value.data = static_cast<char*>(arena.alloc(256*sizeof(char)));
    value.size = 0;

    TimeThroughput("parsePoints", static_cast<u64>(Stat.st_size));

    String contents;
    contents.data = static_cast<char*>(arena.pageAllignedAlloc(MB_16));
    contents.size = 0;

    int fd = open(path.data(), O_RDONLY);

    if(fd == -1) {
        perror("Error opening the file!");
        return { nullptr, 0 };
    }

    u64 total = static_cast<u64>(Stat.st_size);

    bool readingField = false;
    bool readingValue = false;
    i64 currentPair = -1;

    for (uint64_t off = 0; off < total; off += MB_16) {
        size_t chunk = static_cast<size_t>(std::min<uint64_t>(MB_16, total - off));

        if (!mapFileToString(fd, contents, off, chunk)) {
            perror("mapFileToString");
            return { nullptr, 0 };
        }

        contents.size = chunk;
        for (u64 i = 0; i < contents.size; ++i) {
            switch (contents[i]) {
                case '{':
                    currentPair++;
                    parsedPairs[currentPair] = {0, 0, 0, 0};
                    continue;
                case ':':
                    readingValue = true;
                    continue;
                case ',':
                case '}':
                    readingValue = false;
                    break;
                case '"':
                    readingField ^= 1;
                    continue;
                case '[':
                case ']':
                    continue;
                default:
                    break;
            }

            if (std::isspace(contents[i])) continue;

            if (readingField) {
                field.data[field.size] = contents.data[i];
                field.size++;
            }

            if (readingValue) {
                value.data[value.size] = contents.data[i];
                value.size++;
            }

            if (!readingField && !readingValue) {
                if(field == "x0") {
                    std::from_chars(value.data, value.data + value.size, parsedPairs[currentPair].X0);
                } else if (field == "y0") {
                    std::from_chars(value.data, value.data + value.size, parsedPairs[currentPair].Y0);
                } else if (field == "x1") {
                    std::from_chars(value.data, value.data + value.size, parsedPairs[currentPair].X1);
                } else if (field == "y1") {
                    std::from_chars(value.data, value.data + value.size, parsedPairs[currentPair].Y1);
                }

                field.size = 0;
                value.size = 0;
            }
        }
    }

    close(fd);
    return {parsedPairs, static_cast<u64>(currentPair+1)};
}
