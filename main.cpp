#include <stdio.h>
#include <stdlib.h>
#include "FileOperations.h"
#include "Tree.h"
#include "Lexer.h"

const size_t BEGINING_SIZE  = 10;
const char *NAME_INPUT_FILE = "data.txt";

int main()
{
    FILE *finput = fopen(NAME_INPUT_FILE, "r");

    int numberBytesSize = GetFileSize(finput);
    char *str = (char*)calloc(numberBytesSize + 1, sizeof(char));
    str = (char*)ReadFile(finput, str, numberBytesSize);

    Lexer lexer = {};
    lexer.token = (Token*)calloc(BEGINING_SIZE, sizeof(Token));
    lexer.capacity = BEGINING_SIZE;
    lexer.errorCode = NO_ERROR;

    LexicalAnalysis(str, &lexer);
    if (lexer.errorCode != NO_ERROR)
    {
        printf("ERROR\n");
    }
    else
    {
        for (size_t i = 0; i < lexer.curToken; i++)
        {
            printf("%s\n", lexer.token[i].str);
        }
    }

    free(str);
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
