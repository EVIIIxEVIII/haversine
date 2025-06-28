#include <stdlib.h>
#include <stdio.h>

#include "timer.cpp"

typedef double f64;

int main(int argc, char** argv) {
    u64 millisecondsToWait = 1000;

    if(argc == 2) {
        millisecondsToWait = atol(argv[1]);
    }

    u64 cpuFreq = estimateCPUFreq(millisecondsToWait);
    printf("CPU Frequency (guess): %lu\n", cpuFreq);
}
