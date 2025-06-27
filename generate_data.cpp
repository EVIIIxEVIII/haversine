#include <iostream>
#include <cmath>
#include <random>
#include <sstream>
#include <string>
#include <fstream>
#include <array>

#define EARTH_RADIUS 6378
#define CLUSTERS 16

typedef double f64;
typedef int32_t i32;

static f64 square(f64 x) {
    return x*x;
}

static f64 radiansFromDegrees(f64 theta) {
    return M_PI / 180 * theta;
}

static f64 haversine(f64 X0, f64 Y0, f64 X1, f64 Y1, f64 radius) {
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

int main(int argc, char** argv) {
    if (argc == 1) {
        std::cout << "Usage: [seed] [number of points] [output file]" << "\n";
        return 1;
    }

    if (argc != 4)  {
        std::cerr << "Please provide all the arguments!" << "\n";
        return 1;
    }

    i32 seed               = std::atoi(argv[1]);
    i32 pointsCount        = std::atoi(argv[2]);
    std::string outputFile = argv[3];
    std::mt19937 gen(seed);

    std::uniform_real_distribution<> xDist(0, 180);
    std::uniform_real_distribution<> yDist(0,  90);

    std::vector<std::array<std::uniform_real_distribution<>, 2>> clusters;
    for (int i = 0; i < CLUSTERS; ++i) {
        f64 randX = xDist(gen);
        f64 randY = yDist(gen);
        std::uniform_real_distribution<> xCluster(-randX, randX);
        std::uniform_real_distribution<> yCluster(-randY,  randY);

        std::array<std::uniform_real_distribution<>, 2> distributions = {
            std::move(xCluster), std::move(yCluster)
        };

        clusters.push_back(distributions);
    }

    std::string jsonOutput = "[ \n";
    std::vector<f64> answers(pointsCount);

    f64 haversineSum = 0.0;
    for (i32 i = 0; i < pointsCount; ++i) {
        f64 X0 = clusters[i % CLUSTERS][0](gen);
        f64 X1 = clusters[i % CLUSTERS][0](gen);
        f64 Y0 = clusters[i % CLUSTERS][1](gen);
        f64 Y1 = clusters[i % CLUSTERS][1](gen);

        f64 answer = haversine(X0, Y0, X1, Y1, EARTH_RADIUS);
        answers.at(i) = answer;
        haversineSum += answer;

        std::ostringstream oss;
        oss << "{" <<
            "\"x0\":" << X0 << "," <<
            "\"y0\":" << Y0 << "," <<
            "\"x1\":" << X1 << "," <<
            "\"y1\":" << Y1 <<
            "}" << ((i == pointsCount - 1) ? "" : ",") << "\n";

        std::string jsonRow = oss.str();
        jsonOutput.append(jsonRow);
    }

    std::cout << "Average haversine: " << haversineSum / pointsCount << "\n";

    jsonOutput.append(" ]");

    std::ofstream outFile(outputFile);
    if (!outFile) {
        return 1;
    }

    outFile << jsonOutput;
    outFile.close();

    std::cout << "Json file saved\n";

    std::ofstream out(std::string("bin_") + outputFile + ".bin", std::ios::binary);
    if (!out) return 1;

    size_t size = answers.size();
    out.write(reinterpret_cast<const char*>(&size), sizeof(size));
    out.write(reinterpret_cast<const char*>(answers.data()), size * sizeof(f64));

    out.close();
    std::cout << "Bin file saved\n";
}
