#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <sched.h>
#include <type_traits>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "rep_tester.cpp"

typedef uint8_t u8;
typedef int64_t i64;

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

extern "C" void read_4x2(int count, u8* data);
extern "C" void read_8x2(int count, u8* data);
extern "C" void read_16x2(int count, u8* data);
extern "C" void read_32x2(int count, u8* data);

extern "C" void test_L1(int count, u8* data);
extern "C" void test_L2(int count, u8* data);
extern "C" void test_L3(int count, u8* data);
extern "C" void test_ram(int count, u8* data);
extern "C" int test_cache(i64 count, u8* data, i64 chunk);

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

void getThroughputGraph(Buffer& buffer, Tester& tester) {
    int fd = open("log.csv", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open");
        return;
    }

    if (dup2(fd, 1) < 0) {
        perror("dup2");
        return;
    }

    int ram = 268435456;

    for (int dataSize = 1000; dataSize < ram; dataSize+=10240) {
        uint64_t start = readCPUTimer();
        int dataProcessed = test_cache(buffer.count, buffer.data, dataSize);
        uint64_t end = readCPUTimer();
        uint64_t elapsed = end - start;
        f64 throughput = getThroughput((f64)elapsed / tester.cpuFreq, dataProcessed);
        printf("%d, %f\n", dataSize, throughput);
    }

    reset(tester);
    close(fd);
}


void testAsmLoops(Buffer& buffer, Tester& tester) {
    testLoop(test_L1, tester, buffer,  "L1 cache speed");
    testLoop(test_L2, tester, buffer,  "L2 cache speed");
    testLoop(test_L3, tester, buffer,  "L3 cache speed");
    testLoop(test_ram, tester, buffer,  "RAM cache speed");

    //testLoop(read_4x2, tester, buffer,  "4 byte read");
    //testLoop(read_8x2, tester, buffer,  "8 byte read");
    //testLoop(read_16x2, tester, buffer, "16 byte read");
    //testLoop(read_32x2, tester, buffer, "32 byte read");

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
    pin_to_core(1);

    uint64_t cpuFreq = estimateCPUFreq(100);

    Tester tester              = Tester{};
    tester.tryForTime          = 2;
    tester.cpuFreq             = cpuFreq;
    tester.minTime             = INT_MAX;
    tester.maxTime             = 0;
    tester.timeSinceLastUpdate = 0;
    tester.totalTime           = 0;
    tester.totalCount          = 0;

    Buffer buffer;
    buffer.count = 1024 * 1024 * 1024;
    buffer.data = (u8*)malloc(buffer.count);
    for (size_t i = 0; i < buffer.count; i++) {
        buffer.data[i] = (u8)i;
    }
    printf("\n---- Unaligned ---\n");
    testAsmLoops(buffer, tester);
    free(buffer.data);

    printf("\n---- Aligned ---\n");
    buffer.data = (u8*)aligned_alloc(64, buffer.count);
    for (size_t i = 0; i < buffer.count; i++) {
        buffer.data[i] = (u8)i;
    }
    testAsmLoops(buffer, tester);

    return 0;
}
