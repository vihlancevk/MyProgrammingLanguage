#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FileOperations.h"
#include "Tree.h"
#include "Tokenizer.h"
#include "Parser.h"

const size_t BEGINING_SIZE  = 10;
const char *NAME_INPUT_FILE = "data.txt";

int main()
{
    FILE *finput = fopen(NAME_INPUT_FILE, "r");

    int numberBytesSize = GetFileSize(finput);
    char *str = (char*)calloc(numberBytesSize + 1, sizeof(char));
    str = (char*)ReadFile(finput, str, numberBytesSize);

    Lexer lexer = {};
    lexer.tokens = (Token*)calloc(BEGINING_SIZE, sizeof(Token));
    lexer.capacity = BEGINING_SIZE;
    lexer.errorCode = NO_ERROR;

    Tokenizer(str, &lexer);
    /*if (lexer.errorCode != NO_ERROR)
    {
        printf("ERROR\n");
    }
    else
    {
        for (size_t i = 0; i < lexer.curToken; i++)
        {
            printf("key word : %d\n", lexer.tokens[i].keyword);
            printf("id : %s\n", lexer.tokens[i].id);
            printf("value : %g\n\n", lexer.tokens[i].value);
        }
    }*/

    Parser parser = {};
    parser.tokens = lexer.tokens;
    Tree_t tree = {};
    TreeCtor(&tree);
    parser.tree = &tree;
    parser.tree = SyntacticAnalysis(&parser);
    TreeDump(parser.tree);

    free(str);
    free(lexer.tokens);
    //TreeDtor(&tree);
    fclose(finput);
    return 0;
}
