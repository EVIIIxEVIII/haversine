#include <stdio.h>
#include <array>

#pragma once
#include "timer.cpp"

typedef double f64;

struct Anchor {
    const char* name;
    u64 time;
};

std::array<Anchor, 100> anchors;
size_t counter = 0;
u64 totalStart = 0;
u64 totalEnd = 0;

struct TimeScope {
    TimeScope(const char* funcName): funcName(funcName) {
        start = readCPUTimer();
    }

    ~TimeScope() {
        finish = readCPUTimer();
        u64 time = finish - start;
        anchors[counter] = Anchor{
            funcName,
            time
        };

        counter++;
    }

    const char* funcName;
    u64 start;
    u64 finish;
};

void startProfiling() {
    counter = 0;
    anchors = {};
    totalStart = readCPUTimer();
}

void endProfilingAndPrint() {
    totalEnd = readCPUTimer();
    u64 cpuFreq = estimateCPUFreq(1000);
    u64 total = totalEnd - totalStart;

    printf("Total time:  %lu %.9g ms (CPU freq: %lu)\n", total, (f64)total/(f64)cpuFreq*1000., cpuFreq);
    for (u32 i = 0; i < counter; ++i) {
        printf("    %s took %lu (%f %%)\n", anchors[i].name, anchors[i].time, (f64)anchors[i].time / (f64)total * 100);
    }
}

#define TimeFunction() TimeScope time_scope(__func__)
#define TimeBlock(name) TimeScope time_scope(name)
