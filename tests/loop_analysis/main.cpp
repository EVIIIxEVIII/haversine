#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <sched.h>
#include <type_traits>
#include <sys/stat.h>

#include "rep_tester.cpp"

typedef uint8_t u8;

extern "C" void MOVAllBytesASM(int count, u8* data);
extern "C" void NOPAllBytesASM(int count);
extern "C" void CMPAllBytesASM(int count);
extern "C" void CMPAllBytesASMMisaligned(int count);
extern "C" void DECAllBytesASM(int count);

extern "C" void read_1(int count, u8* data);
extern "C" void read_2(int count, u8* data);
extern "C" void read_3(int count, u8* data);
extern "C" void read_4(int count, u8* data);

extern "C" void write_1(int count, u8* data);
extern "C" void write_2(int count, u8* data);
extern "C" void write_3(int count, u8* data);

extern "C" void read_1x2(int count, u8* data);
extern "C" void read_8x2(int count, u8* data);
extern "C" void read_1_4x2(int count, u8* data);

struct Buffer {
    size_t count;
    u8 *data;
};

static void pin_to_core(int cpu) {
    cpu_set_t set; CPU_ZERO(&set); CPU_SET(cpu, &set);
    sched_setaffinity(0, sizeof(set), &set);
}

void test_c_loop(int count, u8 *data) {
    u8* ptr = data;
    for (int i = 0; i < count; i+=3) {
        u8* nptr = ptr+i;
        *(nptr) = (u8)i;
        *(nptr+1) = (u8)i+1;
        *(nptr+2) = (u8)i+2;
    }
}

template <typename T>
constexpr bool always_false = false;

template <typename Fn>
void testLoop(Fn fn, Tester& tester, Buffer& buffer, const char* label) {
    while (shouldTest(tester)) {
        uint64_t start = readCPUTimer();
        if constexpr (std::is_invocable_v<Fn, int>) {
            fn(buffer.count);
        } else if constexpr (std::is_invocable_v<Fn, int, u8*>) {
            fn(buffer.count, buffer.data);
        } else {
            static_assert(always_false<Fn>, "Unsupported function signature");
        }
        uint64_t end = readCPUTimer();
        uint64_t elapsed = end - start;
        addTimeToTester(tester, elapsed);
    }

    printResult(tester, label, buffer.count * sizeof(u8));
    reset(tester);
}

void testAsmLoops(Buffer& buffer, Tester& tester) {
    testLoop(read_1_4x2, tester, buffer, "read eax and al 2 times");
    testLoop(read_1x2, tester, buffer, "read 1 byte 2 times");
    testLoop(read_8x2, tester, buffer, "read 8 bytes 2 times");

    //testLoop(write_1, tester, buffer, "1 write");
    //testLoop(write_2, tester, buffer, "2 write");
    //testLoop(write_3, tester, buffer, "3 write");

    //testLoop(read_1, tester, buffer, "1 read");
    //testLoop(read_2, tester, buffer, "2 read");
    //testLoop(read_3, tester, buffer, "3 read");
    //testLoop(read_4, tester, buffer, "4 read");

    //testLoop(test_c_loop, tester, buffer, "Test C loop");
    //testLoop(CMPAllBytesASMMisaligned, tester, buffer, "CMPAllBytesASMMisaligned");
    //testLoop(MOVAllBytesASM, tester, buffer, "MOVAllBytesASM");
    //testLoop(CMPAllBytesASM, tester, buffer, "CMPAllBytesASM");
    //testLoop(NOPAllBytesASM, tester, buffer, "NOPAllBytesASM");
    //testLoop(DECAllBytesASM, tester, buffer, "DECAllBytesASM");
}

int main() {
    pin_to_core(0);
    uint64_t cpuFreq = estimateCPUFreq(100);

    Tester tester              = Tester{};
    tester.tryForTime          = 2;
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
