#include <malloc.h>
#include <stdbool.h>
#include <string.h>

bool isAllWhitespace(char* string);
int  getIndentAmount(char* line);
void updatePosition(int* depth, int* indentedLineCount, int* indentedLineCountSize, char* line);

const int TAB_WIDTH = 4; // assume 4 spaces per tab

int main(int argc, char* args[]) {
    if (argc != 2) {
        fprintf(stderr, "Invalid use. Usage: %s [FILENAME]\n", args[0]);
        return 1;
    }

    FILE* fh = fopen(args[1], "r");
    if (fh == NULL) {
        fprintf(stderr, "Unable to open file %s for reading\n", args[1]);
        return 1;
    }

    char line[512];                    // I pray 512 characters is enough for everyone...
    int  gutterSize            = 0;
    int* indentedLineCount     = NULL;
    int  indentedLineCountSize = 24;
    int  depth                 = 0;

    indentedLineCount = realloc(indentedLineCount, indentedLineCountSize * sizeof(int));

    // look for the longest line so we can set the gutter width

    while (fgets(line, 512, fh)) {
        updatePosition(&depth, indentedLineCount, &indentedLineCountSize, line);

        int increases[4] = {9, 99, 999, 9999};
        int currentSize = 0;
        for (int i = 0; i < indentedLineCountSize; ++i) {
            if (indentedLineCount[i] == 0) {
                continue;
            }

            currentSize += 2; // 1 for the dot, 1 for the number

            // if number is 10 then it needs another byte of space, same for 100, etc.
            for (int j = 0; j < sizeof(increases) / sizeof(int); ++j) {
                if (indentedLineCount[i] > increases[j]) {
                    ++currentSize;
                } else {
                    break;
                }
            }
        }

        --currentSize; // for stripping the first dot

        if (currentSize > gutterSize) {
            gutterSize = currentSize;
        }
    }

    // reset

    fseek(fh, 0, SEEK_SET);
    for (int i = 0; i < indentedLineCountSize; ++i) {
        indentedLineCount[i] = 0;
    }

    // print each line with its line number

    char lineNumber[gutterSize];
    while (fgets(line, 512, fh)) {
        updatePosition(&depth, indentedLineCount, &indentedLineCountSize, line);

        sprintf(lineNumber, "%s", "");
        for (int i = 0; i < indentedLineCountSize; ++i) {
            if (indentedLineCount[i] != 0) {
                sprintf(lineNumber, "%s.%d", lineNumber, indentedLineCount[i]);
            }
        }

        fprintf(stdout, "%-*s | %s", gutterSize, lineNumber + 1, line);
    }

    return 0;
}


int getIndentAmount(char* string) {
    int indentAmount = 0;
    size_t stringLength = strlen(string) - 1;
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
    size_t stringLength = strlen(string) - 1;
    for (int i = 0; i < stringLength; ++i) {
        if (string[i] != ' ' && string[i] != '\t') {
            return false;
        }
    }

    return true;
}

/**
 * Given the following example code:
 * int main() {
 *     foo();
 * }
 *
 * Then each call to this function with a new line will update the position like the following:
 *
 * int main() {    [+1, 0, 0, 0,  0]  No indentation so +1 to indentLineCount[0]
 *     foo();      [ 1, 0, 0, 0, +1]  4 space characters, so +1 to indentLineCount[4]
 * }               [+1, 0, 0, 0, -1]  No indentation so +1 to indentLineCode[0] and set all subsequent values to 0
 *
 * result:         [ 2, 0, 0, 0,  0]
 */
void updatePosition(int* depth, int* indentedLineCount, int* indentedLineCountSize, char* line) {
    if (!isAllWhitespace(line)) {
        *depth = getIndentAmount(line);
        for (int i = *depth + 1; i < *indentedLineCountSize; i++) {
            indentedLineCount[i] = 0;
        }
    }

    // resize the array if necessary

    if (*depth >= *indentedLineCountSize) {
        indentedLineCountSize += 8;
        indentedLineCount = realloc(indentedLineCount, *indentedLineCountSize * sizeof(int));
    }

    ++indentedLineCount[*depth];
}
