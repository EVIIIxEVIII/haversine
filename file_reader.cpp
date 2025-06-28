#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>

typedef double f64;
struct Pair {
    f64 X0; f64 Y0; f64 X1; f64 Y1;
};

struct Answers {
    std::vector<f64> data;
    f64              haversineSum;
};
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
