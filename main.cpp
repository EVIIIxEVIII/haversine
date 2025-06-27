#include <cctype>
#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <cmath>
#include <assert.h>
#include <sstream>

typedef double f64;

struct Pair {
    f64 X0; f64 Y0; f64 X1; f64 Y1;
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

std::vector<f64> readAnswer(std::string path) {

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
                break;
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
            default:
                continue;
        }
        assert(readingValue != readingField && ("Something went wrong, we can't be reading the value and the field at the same time!"));

        if (std::isspace(contents.at(i))) continue;
        if(readingField) {
            field.push_back(contents.at(i));
        }

        if (readingValue) {
            value.push_back(contents.at(i));
        }

        if (!readingField && !readingValue) {
            if(field == "X0") {
                parsedPairs.at(currentPair).X0 = std::stod(value);
            } else if (field == "Y0") {
                parsedPairs.at(currentPair).Y0 = std::stod(value);
            } else if (field == "X1") {
                parsedPairs.at(currentPair).X1 = std::stod(value);
            } else if (field == "Y1") {
                parsedPairs.at(currentPair).Y1 = std::stod(value);
            }

            field = "";
            value = "";
        }
    }

    return parsedPairs;
}

int main(int argc, char** argv) {






}

