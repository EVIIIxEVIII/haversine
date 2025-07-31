#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <sched.h>
#include <sys/stat.h>
#include "rep_tester.cpp"

typedef uint8_t u8;

extern "C" void MOVAllBytesASM(int count, u8* data);
extern "C" void NOPAllBytesASM(int count);
extern "C" void CMPAllBytesASM(int count);
extern "C" void DECAllBytesASM(int count);

struct Buffer {
    size_t count;
    u8 *data;
};

static void pin_to_core(int cpu) {
    cpu_set_t set; CPU_ZERO(&set); CPU_SET(cpu, &set);
    sched_setaffinity(0, sizeof(set), &set);
}

void test_c_loop(Buffer& buffer, Tester& tester) {
    uint64_t start0 = tsc_start();
    u8* ptr = buffer.data;
    for (int i = 0; i < buffer.count; i++) {
        *(ptr+i) = (u8)i;
    }
    uint64_t end0 = tsc_stop();
    uint64_t elapsed0 = end0 - start0;
    printf("Elapsed: %lu\n", elapsed0);
    printf("Data count: %lu \n", buffer.count);
    addTimeToTester(tester, elapsed0);
    printResult(tester, "Test C loop", buffer.count);
    reset(tester);
}

void testAsmLoops(Buffer& buffer, Tester& tester) {
    uint64_t start1 = tsc_start();
    MOVAllBytesASM(buffer.count, buffer.data);
    uint64_t end1 = tsc_stop();
    uint64_t elapsed1 = end1 - start1;
    printf("Elapsed: %lu\n", elapsed1);
    printf("Data count: %lu \n", buffer.count);
    addTimeToTester(tester, elapsed1);
    printResult(tester, "Test MOVAllBytesASM", buffer.count);
    reset(tester);

    test_c_loop(buffer, tester);

    uint64_t start2 = tsc_start();
    NOPAllBytesASM(buffer.count);
    uint64_t end2 = tsc_stop();
    uint64_t elapsed2 = end2 - start2;
    printf("Elapsed: %lu\n", elapsed2);
    printf("Data count: %lu \n", buffer.count);
    addTimeToTester(tester, elapsed2);
    printResult(tester, "Test NOPAllBytesASM", buffer.count);
    reset(tester);

    uint64_t start3 = readCPUTimer();
    CMPAllBytesASM(buffer.count);
    uint64_t end3 = readCPUTimer();
    uint64_t elapsed3 = end3 - start3;
    printf("Elapsed: %lu\n", elapsed3);
    printf("Data count: %lu \n", buffer.count);
    addTimeToTester(tester, elapsed3);
    printResult(tester, "Test CMPAllBytesASM", buffer.count);
    reset(tester);

    uint64_t start = readCPUTimer();
    DECAllBytesASM(buffer.count);
    uint64_t end = readCPUTimer();
    uint64_t elapsed = end - start;
    addTimeToTester(tester, elapsed);
    printResult(tester, "Test DECAllBytesASM", buffer.count);
    reset(tester);
}

int main() {
    pin_to_core(0);
    uint64_t cpuFreq = estimateCPUFreq(100);

    Tester tester              = Tester{};
    tester.tryForTime          = 5;
    tester.cpuFreq             = cpuFreq;
    tester.minTime             = INT_MAX;
    tester.maxTime             = 0;
    tester.timeSinceLastUpdate = 0;
    tester.totalTime           = 0;
    tester.totalCount          = 0;

    printf("rdtsc freq: %lu", tester.cpuFreq);
    Buffer buffer;
    buffer.count = 998794351;
    buffer.data = (u8*)malloc(buffer.count);
    buffer.data[0] = 0;
    for (size_t i = 0; i < buffer.count; i += 4096) {
        buffer.data[i] = 0;
    }

    testAsmLoops(buffer, tester);

    return 0;
}
