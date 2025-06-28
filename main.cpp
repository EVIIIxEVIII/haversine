#include <cctype>
#include <iostream>
#include <cmath>
#include <assert.h>
#include <iomanip>

#include "file_reader.cpp"
#include "timer.cpp"

#define EARTH_RADIUS 6378

static f64 square(f64 x) {
    return x*x;
}

static f64 radiansFromDegrees(f64 theta) {
    return M_PI / 180 * theta;
}

static f64 haversine(f64 X0, f64 Y0, f64 X1, f64 Y1, f64 radius) {
    assert((X0 > -180 && X0 < 180) && (X1 > -180 && X1 < 180));
    assert((Y0 > -90 && Y0 < 90)   && (Y1 > -90 && Y1 < 90));

    f64 lat1 = Y0;
    f64 lat2 = Y1;

    f64 lon1 = X0;
    f64 lon2 = X1;

    f64 dLat = radiansFromDegrees(lat2 - lat1);
    f64 dLon = radiansFromDegrees(lon2 - lon1);

    lat1 = radiansFromDegrees(lat1);
    lat2 = radiansFromDegrees(lat2);

    f64 a = square(sin(dLat/2.0)) + cos(lat1)*cos(lat2)*square(sin(dLon/2.0));
    f64 c = 2.0*asin(sqrt(a));

    return radius * c;
}


void validate(f64 computed, f64 answer) {
    std::cout << "Computed haversine sum: " << computed << "\n";
    std::cout << "Correct haversine sum: " << answer << "\n";

    if (fabs(computed - answer) < 1e-6) {
        std::cout << "\n\n\n" << "THE ANSWER IS CORRECT!" << "\n\n\n";
    } else {
        std::cout << "\n\n\n" << "THE ANSWER IS WRONG!" << "\n\n\n";
    }
}

int main(int argc, char** argv) {
    std::cout << std::fixed << std::setprecision(15);

    if (argc == 1) {
        std::cout << "Usage: [pairs.json] [answers.bin]" << "\n";
        return 0;
    }

    if (argc != 3) {
        std::cout << "Please input all arguments!" << "\n";
        return 0;
    }


    u64 cpuFreq = estimateCPUFreq(1000);
    const char* jsonFile = argv[1];
    const char* answersFile  = argv[2];

    u64 parsePointsStart = readCPUTimer();
    std::vector<Pair> pairs = parsePoints(jsonFile);
    u64 parsePointsEnd = readCPUTimer();
    u64 parsePointsTime = parsePointsEnd - parsePointsStart;

    printf("Input size: %lu \n", pairs.size());

    u64 readAnswersStart = readCPUTimer();
    Answers answers = readAnswers(answersFile);
    u64 readAnswersEnd = readCPUTimer();
    u64 readAnswersTime = readAnswersEnd - readAnswersStart;

    u64 haversineStart = readCPUTimer();
    f64 haversineSum = 0;
    for (int i = 0; i < pairs.size(); ++i) {
        f64 val = haversine(
            pairs.at(i).X0,
            pairs.at(i).Y0,
            pairs.at(i).X1,
            pairs.at(i).Y1,
            EARTH_RADIUS
        );

        haversineSum += val;
    }
    u64 haversineEnd = readCPUTimer();
    u64 haversineTime = haversineEnd - haversineStart;

    u64 total = haversineTime + readAnswersTime + parsePointsTime;

    printf("\n\n");
    printf("Total time: %f ms (CPU freq: %lu)\n", (f64)total/(f64)cpuFreq*1000, cpuFreq);
    printf("Parse points time %lu (%f %%) \n", parsePointsTime, (f64)parsePointsTime/total * 100.);
    printf("Read answers time %lu (%f %%) \n", readAnswersTime, (f64)readAnswersTime/total * 100.);
    printf("Haversine time %lu (%f %%) \n", haversineTime, (f64)haversineTime/total * 100.);
    printf("\n\n");

    validate(haversineSum, answers.haversineSum);
    return 0;
}

