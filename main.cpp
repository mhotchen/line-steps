#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>
#include <iomanip>

bool isAllWhitespace(char* string);
int  getIndentAmount(char* line);

const int TAB_WIDTH = 4; // assume 4 spaces per tab

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
    lines.resize(512, Line());

    /*
     * Given the following example code:
     * int main() {
     *     foo();
     * }
     *
     * Then the following loop through each line will affect the indentedLineCount data structure like so:
     *
     * int main() {    [+1, 0, 0, 0,  0]  No indentation so +1 to indentLineCount[0]
     *     foo();      [ 1, 0, 0, 0, +1]  4 space characters, so +1 to indentLineCount[4]
     * }               [+1, 0, 0, 0, -1]  No indentation so +1 to indentLineCode[0] and set all subsequent values to 0
     *
     * result:         [ 2, 0, 0, 0,  0]
     */

    std::vector<int> indentedLineCount;
    indentedLineCount.resize(16, 0);

    int  currentIndent      = 0;
    int  longestDepthString = 0;
    int  lineNumber         = 0;
    char line[512];              // I pray this is enough for everyone...

    std::ifstream file(args[1]);
    if (!file.good()) {
        std::cerr << "File " << args[1] << " not found or not readable" << std::endl;
        return 1;
    }

    while (file.good()) {
        file.getline(line, 512);

        // Calculate indent and increment the correct position in indentedLineCount

        if (!isAllWhitespace(line)) {
            currentIndent = getIndentAmount(line);

            for (int i = currentIndent + 1; i < indentedLineCount.size(); ++i) {
                indentedLineCount[i] = 0;
            }
        }

        if (indentedLineCount.size() <= currentIndent) {
            indentedLineCount.resize(currentIndent + 8, 0);
        }

        indentedLineCount[currentIndent]++;

        // Generate the string for placing in the gutter

        std::string depthString;
        for (int i = 0; i < indentedLineCount.size(); ++i) {
            if (indentedLineCount[i] == 0) {
                continue;
            }

            depthString = depthString + "." + std::to_string(indentedLineCount[i]);
        }

        depthString = depthString.substr(1);

        lines[lineNumber].line = line;
        lines[lineNumber].number = depthString;

        ++lineNumber;

        // Calculate gutter size

        if (depthString.length() > longestDepthString) {
            longestDepthString = depthString.length();
        }

        // Resize the lines vector if necessary

        if (lineNumber >= lines.size()) {
            lines.resize(lineNumber + 512, Line());
        }
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


int getIndentAmount(char* string) {
    int indentAmount = 0;
    int stringLength = strlen(string);
    for (int i = 0; i < stringLength; ++i) {
        if (string[i] == ' ') {
            ++indentAmount;
        } else if (string[i] == '\t') {
            indentAmount += TAB_WIDTH;
        } else {
            break;
        }
    }

    return indentAmount;
}


bool isAllWhitespace(char* string) {
    int stringLength = strlen(string);
    for (int i = 0; i < stringLength; ++i) {
        if (string[i] != ' ' && string[i] != '\t') {
            return false;
        }
    }

    return true;
}