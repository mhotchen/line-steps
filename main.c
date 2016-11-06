#include <errno.h>
#include <malloc.h>
#include <memory.h>
#include <stdbool.h>
#include <stdlib.h>

#define TAB_WIDTH 4
#define MAX_LINE_SIZE 512 /* I pray 512 characters is enough for everyone... */

bool isAllWhitespace(char line[MAX_LINE_SIZE]);
int  getIndentAmount(char line[MAX_LINE_SIZE]);
int  getLineNumberSize(int indentedLineCountSize, int indentedLineCount[indentedLineCountSize]);
void updatePosition(int *depth, int *indentedLineCountSize, int *indentedLineCount, char line[MAX_LINE_SIZE]);
void buildLineNumberString(char string[], int indentedLineCountSize, int indentedLineCount[indentedLineCountSize]);

int
main(int argc, char *args[argc])
{
    if (argc != 2) {
        fprintf(stderr, "Invalid use. Usage: %s [FILENAME]\n", args[0]);
        exit(EXIT_FAILURE);
    }

    FILE* fh = fopen(args[1], "r");
    if (fh == NULL) {
        fprintf(stderr, "%s: Unable to open file %s for reading (%s)\n", args[0], args[1], strerror(errno));
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE_SIZE]   = {0};
    int  gutterSize            = 0;
    int  indentedLineCountSize = 24;
    int  *indentedLineCount    = malloc(indentedLineCountSize * sizeof(int));
    int  depth                 = 0;

    /* Find the longest line so we can set the gutter width */

    int max(const int a, const int b) {return a > b ? a : b;}
    while (fgets(line, MAX_LINE_SIZE, fh)) {
        updatePosition(&depth, &indentedLineCountSize, indentedLineCount, line);
        gutterSize = max(getLineNumberSize(indentedLineCountSize, indentedLineCount), gutterSize);
    }

    if (gutterSize > 100) {
        fprintf(stderr, "%s: Indentation is too deep\n", args[0]);
        exit(EXIT_FAILURE);
    }

    /* Reset */

    depth = 0;
    fseek(fh, 0, SEEK_SET);
    for (int i = 0; i < indentedLineCountSize; ++i) {
        indentedLineCount[i] = 0;
    }

    /* Print each line with its line number */

    char lineNumber[gutterSize];
    while (fgets(line, MAX_LINE_SIZE, fh)) {
        updatePosition(&depth, &indentedLineCountSize, indentedLineCount, line);

        buildLineNumberString(lineNumber, indentedLineCountSize, indentedLineCount);
        fprintf(stdout, "%-*s | %s", gutterSize, lineNumber, line);
    }

    fprintf(stdout, "\n");

    exit(EXIT_SUCCESS);
}

int
getIndentAmount(char line[MAX_LINE_SIZE])
{
    int indentAmount = 0;

    for (int i = 0; i < MAX_LINE_SIZE; ++i) {
        switch (line[i]) {
            case ' ':  ++indentAmount;            break;
            case '\t': indentAmount += TAB_WIDTH; break;
            default:   return indentAmount;
        }
    }

    return indentAmount;
}

int
getLineNumberSize(int indentedLineCountSize, int indentedLineCount[indentedLineCountSize])
{
    int  currentSize = 0;
    char *asString[MAX_LINE_SIZE];

    for (int i = 0; i < indentedLineCountSize; ++i) {
        if (indentedLineCount[i] == 0) {
            continue;
        }

        /* The length of the number as a string + an extra character for the dot */
        snprintf((char *)asString, MAX_LINE_SIZE, "%d", indentedLineCount[i]);
        currentSize += strlen((char *)asString) + 1;
    }

    /* Decrement by one to strip the preceding dot */
    return --currentSize;
}

bool
isAllWhitespace(char line[MAX_LINE_SIZE])
{
    for (int i = 0; i < MAX_LINE_SIZE; ++i) {
        switch (line[i]) {
            case ' ':
            case '\t': continue;

            case '\0':
            case '\n':
            case '\r': return true;

            default:   return false;
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
void
updatePosition(int *depth, int *indentedLineCountSize, int *indentedLineCount, char line[MAX_LINE_SIZE])
{
    if (!isAllWhitespace(line)) {
        *depth = getIndentAmount(line);
    }

    /* Resize the array if necessary */

    if ((*depth) > (*indentedLineCountSize)) {
        (*indentedLineCountSize) += ((*depth) + 8);
        indentedLineCount = realloc(indentedLineCount, (*indentedLineCountSize) * sizeof(int));
    }

    ++indentedLineCount[*depth];

    /* Clear values that are deeper than the current depth */

    for (int i = *depth + 1; i < *indentedLineCountSize; i++) {
        indentedLineCount[i] = 0;
    }
}

void
buildLineNumberString(char *string, int indentedLineCountSize, int indentedLineCount[indentedLineCountSize])
{
    string[0] = 0;
    for (int i = 0; i < indentedLineCountSize; ++i) {
        if (indentedLineCount[i] != 0) {
            sprintf(string, "%s.%d", string, indentedLineCount[i]);
        }
    }
    sprintf(string, "%s", string + 1); /* + 1 to skip the leading . character */
}
