#include <iostream>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <malloc.h>
#include <fstream>
#include <sstream>
#include <climits>

#include "rep_testing.cpp"

Tester test_read(const char* fileName, Tester tester) {
    struct stat Stat;
    stat(fileName, &Stat);


    char* buffer = (char*)malloc(Stat.st_size * sizeof(char));
    while (shouldTest(tester)) {
        int fd = open(fileName, O_RDONLY, 0644);

        if(fd == -1) {
            perror("Error opening the file using read!");
            return {};
        }

        u64 start = readCPUTimer();
        int res = read(fd, buffer, Stat.st_size);
        u64 end = readCPUTimer();

        if(res == -1) {
            perror("Error reading the file!");
            return {};
        }

        u64 elapsed = end - start;
        addTimeToTester(tester, elapsed);

        close(fd);
    }

    free(buffer);
    printResult(tester, "Test read", Stat.st_size);
    return tester;
}


Tester test_fread(const char* fileName, Tester tester) {
    struct stat Stat;
    stat(fileName, &Stat);

    char* buffer = (char*)malloc(Stat.st_size * sizeof(char));
    while (shouldTest(tester)) {
        FILE *file = fopen(fileName, "r");

        if(file == NULL) {
            perror("Error opening the file using read!");
            return {};
        }

        u64 start = readCPUTimer();
        int res = fread(buffer, sizeof(char), Stat.st_size / sizeof(char), file);
        u64 end = readCPUTimer();

        if(res == -1) {
            perror("Error reading the file!");
            return {};
        }

        u64 elapsed = end - start;
        addTimeToTester(tester, elapsed);

        fclose(file);
    }

    free(buffer);
    printResult(tester, "Test read", Stat.st_size);
    return tester;

}

Tester test_ifstream(const char* fileName, Tester tester) {
    struct stat Stat;
    stat(fileName, &Stat);

    while (shouldTest(tester)) {
        std::ifstream jsonFile(fileName);

        if (!jsonFile.is_open()) {
            std::cout << "Failed to open json file! \n";
            return {};
        }

        std::stringstream buffer;
        u64 start = readCPUTimer();
        buffer << jsonFile.rdbuf();
        u64 end = readCPUTimer();

        u64 elapsed = end - start;
        addTimeToTester(tester, elapsed);
        std::string contents = buffer.str();
    }

    printResult(tester, "Test read", Stat.st_size);
    return tester;
}

int main (int argc, char** argv) {
    if(argc == 1) {
        printf("Please specify a file! \n");
        return 0;
    }

    const char* fileName = argv[1];

    u64 cpuFreq = estimateCPUFreq(1000);

    Tester tester              = Tester{};
    tester.tryForTime          = 5;
    tester.cpuFreq             = cpuFreq;
    tester.minTime             = INT_MAX;
    tester.maxTime             = 0;
    tester.timeSinceLastUpdate = 0;
    tester.totalTime           = 0;
    tester.totalCount          = 0;

    test_read(fileName, tester);
    test_fread(fileName, tester);
    test_ifstream(fileName, tester);
}
