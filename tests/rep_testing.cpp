
#include "../src/timer.cpp"
#include <stdio.h>

typedef double f64;

struct Tester {
    f64 tryForTime;
    u64 cpuFreq;

    u64 minTime;
    u64 maxTime;

    u64 timeSinceLastUpdate;

    u64 totalTime;
    u64 totalCount;
};


void addTimeToTester(Tester& tester, u64 elapsed) {
    if(tester.minTime > elapsed) {
        tester.minTime = elapsed;
        tester.timeSinceLastUpdate = 0;
    } else {
        tester.timeSinceLastUpdate += elapsed;
    }

    if(tester.maxTime < elapsed) {
        tester.maxTime = elapsed;
    }

    tester.totalTime += elapsed;
    tester.totalCount ++;
}

bool shouldTest(Tester& tester) {
    if(tester.tryForTime < ((f64)tester.timeSinceLastUpdate / (f64)tester.cpuFreq)) {
        return false;
    }

    return true;
}

void printResult(const Tester& tester, const char *functionName) {
    printf("---- %s ----\n", functionName);

    printf("Min time: %fms \n", (f64) tester.minTime / (f64) tester.cpuFreq * 1000.);
    printf("Avg time: %fms \n", ((f64) tester.totalTime / (f64) tester.totalCount) / (f64) tester.cpuFreq * 1000.);
    printf("Max time: %fms \n", (f64) tester.maxTime / (f64) tester.cpuFreq * 1000.);
}
