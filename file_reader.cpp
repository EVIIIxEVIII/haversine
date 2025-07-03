#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>
#include <cstring>

#include "simple_profiler.cpp"

typedef double f64;
struct Pair {
    f64 X0; f64 Y0; f64 X1; f64 Y1;
};

struct Answers {
    std::vector<f64> data;
    f64              haversineSum;
};

std::vector<char> readFileAsBin(std::string path) {
    TimeFunction();
    std::ifstream file(path, std::ios::binary | std::ios::ate);

    if (!file.is_open()) {
        std::cout << "Failed to open answers file! \n";
        return {};
    }

    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(fileSize);

    if (!file.read(buffer.data(), fileSize)) {
        std::cout << "Failed to read file into memory! \n";
        return {};
    }

    return buffer;
}

Answers readAnswers(std::string path) {
    TimeFunction();

    std::vector<char> buffer = readFileAsBin(path);
    if(buffer.empty()) return {};
    const char* data = buffer.data();

    const size_t size = *reinterpret_cast<const size_t*>(data);
    data += sizeof(size_t);

    std::vector<f64> answers(size);

    std::memcpy(answers.data(), data, size * sizeof(f64));

    f64 haversineSum = answers.back();
    answers.pop_back();

    return Answers {
        answers,
        haversineSum
    };
}

std::string readFileAsString(const std::string& path) {
    TimeFunction();
    std::ifstream jsonFile(path);

    if (!jsonFile.is_open()) {
        std::cout << "Failed to open json file! \n";
        return {};
    }

    std::stringstream buffer;
    buffer << jsonFile.rdbuf();

    std::string contents = buffer.str();
    return contents;
}

std::vector<Pair> parsePoints(const std::string& path) {
    TimeFunction();
    std::vector<Pair> parsedPairs;
    std::string field = "";
    std::string value = "";

    std::string contents = readFileAsString(path);
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
