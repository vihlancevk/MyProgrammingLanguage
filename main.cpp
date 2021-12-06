#include <stdio.h>
#include <stdlib.h>
#include "FileOperations.h"
#include "Tree.h"
#include "ProgrammingLanguage.h"

const size_t BEGINING_SIZE  = 10;
const char *NAME_INPUT_FILE = "data.txt";

int main()
{
    Parser parser = {};

    FILE *finput = fopen(NAME_INPUT_FILE, "r");

    int numberBytesSize = GetFileSize(finput);
    parser.str = (char*)calloc(numberBytesSize + 1, sizeof(char));
    parser.str = (char*)ReadFile(finput, parser.str, numberBytesSize);

    parser.node = (Node_t*)calloc(BEGINING_SIZE, sizeof(Node_t));
    parser.capacity = BEGINING_SIZE;

    parser.errorCode = NO_ERROR;

    LexicalAnalysis(&parser);
    if (parser.errorCode != NO_ERROR)
    {
        printf("ERROR\n");
    }
    else
    {
        for (size_t i = 0; i < parser.curSize; i++)
        {
            printf("%d, %g\n", (int)parser.node[i].nodeType, parser.node[i].value);
        }
    }

    free(parser.str);
    free(parser.node);
    fclose(finput);
    return 0;
}
