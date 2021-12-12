#ifndef PROGRAMMING_LANGUAGE_H_
#define PROGRAMMING_LANGUAGE_H_

#include "Tree.h"

const size_t STR_MAX_SIZE = 10;

enum ErrorCode
{
    NO_ERROR,
    SYNTAX_ERROR,
    REALLOC_ERROR
};

union Token
{
    double value;
    NodeType keyword;
    char id[STR_MAX_SIZE];
};

struct Lexer
{
    Token *tokens;
    size_t capacity;
    size_t curToken;
    ErrorCode errorCode;
};

struct UnaryOperation
{
    const char *str;
    size_t strSize;
    double (*operation) (double);
};

void LexicalAnalysis(char *str, Lexer *lexer);

#endif // PROGRAMMING_LANGUAGE_H_

