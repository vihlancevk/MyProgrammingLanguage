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

    Lexer lexer = {};
    lexer.token = (Node_t*)calloc(BEGINING_SIZE, sizeof(Node_t));
    lexer.capacity = BEGINING_SIZE;

    lexer.errorCode = NO_ERROR;
    LexicalAnalysis(&parser, &lexer);
    if (lexer.errorCode != NO_ERROR)
    {
        printf("ERROR\n");
    }
    else
    {
        for (size_t i = 0; i < lexer.curToken; i++)
        {
            printf("%d, %g\n", (int)lexer.token[i].nodeType, lexer.token[i].value);
        }
    }

    free(parser.str);
    for (size_t i = 0; i < lexer.curToken; i++)
    {
        if (lexer.token[i].str != nullptr)
        {
            free(lexer.token[i].str);
        }
    }
    free(lexer.token);
    fclose(finput);
    return 0;
}
