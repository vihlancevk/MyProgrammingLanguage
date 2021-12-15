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

    Parser parser = {};
    parser.tokens = lexer.tokens;
    Tree_t tree = {};
    parser.tree = &tree;
    TreeCtor(parser.tree);
    parser.tree = SyntacticAnalysis(&parser);
    TreeDump(parser.tree);

    free(str);
    free(lexer.tokens);
    TreeDtor(parser.tree);
    fclose(finput);
    return 0;
}

//main.cpp Tokenizer.cpp Parser.cpp Tree.cpp FileOperations.cpp
