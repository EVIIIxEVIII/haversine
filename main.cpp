#include <cctype>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <assert.h>
#include <sstream>
#include <iomanip>

#define EARTH_RADIUS 6378

typedef double f64;

struct Pair {
    f64 X0; f64 Y0; f64 X1; f64 Y1;
};

struct Answers {
    std::vector<f64> data;
    f64              haversineSum;
};

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

Answers readAnswers(std::string path) {
    std::ifstream answersFile(path, std::ios::binary);

    if (!answersFile.is_open()) {
        std::cout << "Failed to open answers file! \n";
        return {};
    }

    size_t size = 0;
    answersFile.read(reinterpret_cast<char*>(&size), sizeof(size));
    std::vector<f64> answers(size);

    for (int i = 0; i < size; ++i) {
        f64 temp = 0;
        answersFile.read(reinterpret_cast<char*>(&answers.at(i)), sizeof(answers.at(i)));
        std::cout << answers.at(i) << "\n";
    }

    f64 haversineSum = answers.back();
    answers.pop_back();
    return Answers {
        answers,
        haversineSum
    };
}

std::vector<Pair> parsePoints(std::string path) {
    std::vector<Pair> parsedPairs;
    std::ifstream jsonFile(path);

    if (!jsonFile.is_open()) {
        std::cout << "Failed to open json file! \n";
        return {};
    }

    std::stringstream buffer;
    buffer << jsonFile.rdbuf();

    std::string contents = buffer.str();
    std::string field = "";
    std::string value = "";

    bool readingField = false;
    bool readingValue = false;

    int currentPair = -1;
    for (int i = 0; i < contents.size(); ++i) {
        switch (contents.at(i)) {
            case '{':
                parsedPairs.push_back({0, 0, 0, 0});
                currentPair++;
                continue;
            case ':':
                readingValue = true;
                continue;
            case ',':
            case '}':
                readingValue = false;
                break;
            case '"':
                readingField = !readingField;
                continue;
            case '[':
            case ']':
                continue;
            default:
                break;
        }
        if (std::isspace(contents.at(i))) continue;

        if(readingField) {
            field.push_back(contents.at(i));
        }

        if (readingValue) {
            value.push_back(contents.at(i));
        }

        if (!readingField && !readingValue) {

            if(field == "x0") {
                parsedPairs.at(currentPair).X0 = std::stod(value);
            } else if (field == "y0") {
                parsedPairs.at(currentPair).Y0 = std::stod(value);
            } else if (field == "x1") {
                parsedPairs.at(currentPair).X1 = std::stod(value);
            } else if (field == "y1") {
                parsedPairs.at(currentPair).Y1 = std::stod(value);
            }

            field.clear();
            value.clear();
        }
    }

    return parsedPairs;
}

int main(int argc, char** argv) {
    std::cout << std::fixed << std::setprecision(15);

    if (argc == 1) {
        std::cout << "Usage: [pairs.json] [answers.bin]";
        return 0;
    }

    if (argc != 3) {
        std::cout << "Please input all arguments!";
        return 0;
    }

    std::string jsonFile = argv[1];
    std::string answersFile  = argv[2];

    std::vector<Pair> pairs = parsePoints(jsonFile);
    Answers answers = readAnswers(answersFile);

    f64 haversineSum = 0;
    for (int i = 0; i < pairs.size(); ++i) {
        f64 val = haversine(
            pairs.at(i).X0,
            pairs.at(i).Y0,
            pairs.at(i).X1,
            pairs.at(i).Y1,
            EARTH_RADIUS
        );

        std::cout << answers.data.at(i) << "  " << val << "\n";
        haversineSum += val;
    }

    std::cout << "Computed haversine sum: " << haversineSum << "\n";
    std::cout << "Correct haversine sum: " << answers.haversineSum << "\n";

    if (fabs(haversineSum - answers.haversineSum) < 1e-6) {
        std::cout << "\n\n\n" << "THE ANSWER IS CORRECT!" << "\n\n\n";
    } else {
        std::cout << "\n\n\n" << "THE ANSWER IS WRONG!" << "\n\n\n";
    }

    return 0;
}

