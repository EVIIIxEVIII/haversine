#include <cctype>
#include <iostream>
#include <cmath>
#include <assert.h>
#include <iomanip>

#include "file_reader.cpp"
#include "simple_profiler.cpp"

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

f64 computeHaversineSum(std::vector<Pair>& pairs, Answers& answers) {
    TimeFunction();
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
    return haversineSum;
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

    startProfiling();
    const char* jsonFile = argv[1];
    const char* answersFile  = argv[2];

    std::vector<Pair> pairs = parsePoints(jsonFile);

    printf("Input size: %lu \n", pairs.size());

    Answers answers = readAnswers(answersFile);
    f64 haversineSum = computeHaversineSum(pairs, answers);

    endProfilingAndPrint();
    validate(haversineSum, answers.haversineSum);
    return 0;
}

