#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>
#include <iomanip>

bool isIndent(char* line, int currentLevel);
bool isUnindent(char* line, int currentLevel);
bool isAllWhitespace(char* string);
int  getIndentAmount(char* line);

struct Line {
    std::string number;
    std::string line;
};

int main(int argc, char* args[]) {
    if (argc != 2) {
        std::cerr << "Invalid use. Usage: " << args[0] << " [FILENAME]" << std::endl;
        return 1;
    }

    std::vector<Line> lines;
    lines.resize(500, Line());

    std::vector<int> numbers;
    numbers.resize(50, 0);

    int currentIndent = 0;
    int longestDepthString = 0;
    int lineNumber = 0;
    int depth = 0;

    std::ifstream file(args[1]);
    if (!file.good()) {
        std::cerr << "File " << args[1] << " not found or not readable" << std::endl;
        return 1;
    }

    while (file.good()) {
        char line[512]; // I pray this is enough for everyone...
        file.getline(line, 512);

        if (isIndent(line, currentIndent)) {
            depth++;
            currentIndent = getIndentAmount(line);
        } else if (isUnindent(line, currentIndent)) {
            numbers[depth] = 0;
            depth--;
            currentIndent = getIndentAmount(line);
        }

        if (numbers.size() < depth) {
            numbers.resize(depth + 5, 0);
        }

        numbers[depth]++;
        std::string depthString;
        for (int i = 0; i < numbers.size(); ++i) {
            if (numbers[i] == 0) {
                break;
            }

            depthString = depthString + "." + std::to_string(numbers[i]);
        }

        depthString = depthString.substr(1);
        if (depthString.length() > longestDepthString) {
            longestDepthString = depthString.length();
        }

        if (lineNumber > lines.size()) {
            lines.resize(lineNumber + 500, Line());
        }

        lines[lineNumber].line = line;
        lines[lineNumber].number = depthString;

        ++lineNumber;
    }

    for (int i = 0; i < lines.size(); ++i) {
        if (lines[i].number == "") {
            break;
        }

        std::cout << std::left << std::setw(longestDepthString) << lines[i].number;
        std::cout << " | " << lines[i].line << std::endl;
    }

    return 0;
}

bool isIndent(char* line, int currentLevel = 0) {
    if (line[0] == '\0') {
        return false;
    }

    if (strlen(line) <= currentLevel) {
        return false;
    }

    if (isAllWhitespace(line)) {
        std::cout << "50" << std::endl;
        return false;
    }

    return getIndentAmount(line) > currentLevel;
}

bool isUnindent(char* line, int currentLevel = 0) {
    if (line[0] == '\0') {
        return false;
    }

    if (currentLevel == 0) {
        return false;
    }

    if (isAllWhitespace(line)) {
        return false;
    }

    return getIndentAmount(line) < currentLevel;
}

int getIndentAmount(char* string) {
    int i = 0;
    for (; i < strlen(string); ++i) {
        if (string[i] != ' ' && string[i] != '\t') {
            break;
        }
    }

    return i;
}

bool isAllWhitespace(char* string) {
    for (int i = 0; i < strlen(string); ++i) {
        if (string[i] != ' ' && string[i] != '\t') {
            return false;
        }
    }

    return true;
}