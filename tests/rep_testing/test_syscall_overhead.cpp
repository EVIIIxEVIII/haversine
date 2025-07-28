#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <climits>

#include "rep_testing.cpp"

void testSyscallOverhead(Tester& tester) {
    while(shouldTest(tester)) {
        int fd = open("/dev/null", O_RDONLY);

        if (fd < 0) {
            perror("open");
            return;
        }

        uint64_t start = readCPUTimer();
        size_t n = read(fd, NULL, 0);
        uint64_t end = readCPUTimer();

        uint64_t elapsed = end - start;
        addTimeToTester(tester, elapsed);

        if (n < 0) {
            perror("read");
        }

        close(fd);
    }

    printResult(tester, "Test syscall overhead", 0);
}

int main() {
    uint64_t cpuFreq = estimateCPUFreq(100);

    Tester tester              = Tester{};
    tester.tryForTime          = 1;
    tester.cpuFreq             = cpuFreq;
    tester.minTime             = INT_MAX;
    tester.maxTime             = 0;
    tester.timeSinceLastUpdate = 0;
    tester.totalTime           = 0;
    tester.totalCount          = 0;

    testSyscallOverhead(tester);

    return 0;
}
