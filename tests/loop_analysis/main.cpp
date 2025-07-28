#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include "rep_tester.cpp"

#define DATA_SIZE 1024

typedef uint8_t u8;

struct Buffer {
    size_t count;
    u8 *data;
};

void loop(Buffer& buffer, Tester& tester) {
    while(shouldTest(tester)) {
        uint64_t start = readCPUTimer();
        for (int i = 0; i < DATA_SIZE; i+=3) {
            buffer.data[i] = (u8)i;
            buffer.data[i+1] = (u8)i+1;
            buffer.data[i+2] = (u8)i+2;
        }
        uint64_t end = readCPUTimer();
        uint64_t elapsed = end - start;
        addTimeToTester(tester, elapsed);
    }

    printResult(tester, "Test syscall overhead", DATA_SIZE * sizeof(u8));
}

int main() {
    uint64_t cpuFreq = estimateCPUFreq(100);

    Tester tester              = Tester{};
    tester.tryForTime          = 3;
    tester.cpuFreq             = cpuFreq;
    tester.minTime             = INT_MAX;
    tester.maxTime             = 0;
    tester.timeSinceLastUpdate = 0;
    tester.totalTime           = 0;
    tester.totalCount          = 0;

    Buffer buffer;
    buffer.data = (u8*)malloc(DATA_SIZE * sizeof(u8));
    buffer.count = DATA_SIZE;
    buffer.data[0] = 0;

    loop(buffer, tester);

    return 0;
}
