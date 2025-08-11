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

String readFileAsString(const std::string& path, Arena& arena) {
    struct stat Stat;
    stat(path.data(), &Stat);
    TimeThroughput("readFileAsString2", static_cast<u64>(Stat.st_size));

    int fd = open(path.data(), O_RDONLY);

    if(fd == -1) {
        perror("Error opening the file!");
        return String { nullptr, 0 };
    }

    String content;
    content.size = static_cast<u64>(Stat.st_size);
    content.data = static_cast<char*>(arena.alloc(static_cast<u64>(Stat.st_size)));

    ssize_t bytesRead = read(fd, content.data, static_cast<size_t>(Stat.st_size));

    if (bytesRead == -1) {
        perror("Error reading the file!");
        return String { nullptr, 0 };
    }

    close(fd);
    return content;
}

Pairs parsePoints(const std::string& path, Arena& targetArena) {
    struct stat Stat;
    stat(path.data(), &Stat);

    TimeThroughput("parsePoints", static_cast<u64>(Stat.st_size));

    u64 GB_1_5 = static_cast<u64>(1.5 * 1024 * 1024 * 1024);

    Arena arena(GB_1_5);
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

    String contents = readFileAsString(path, arena);
    if (!contents.data) {
        return { nullptr, 0 };
    }

    bool readingField = false;
    bool readingValue = false;

    i64 currentPair = -1;
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
                readingValue = false;
                break;
            case '}':
                readingValue = false;
                break;
            case '"':
                readingField = !readingField;
                continue;
            case '[':
            case ']':
                continue;
            default:
                break;
        }

        if (std::isspace(contents[i])) continue;

        if(readingField) {
            field.data[field.size] = contents[i];
            field.size++;
        }

        if (readingValue) {
            value.data[value.size] = contents[i];
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

    return {parsedPairs, static_cast<u64>(currentPair+1)};
}
