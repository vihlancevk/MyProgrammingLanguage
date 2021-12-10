#ifndef PROGRAMMING_LANGUAGE_H_
#define PROGRAMMING_LANGUAGE_H_

#include "Tree.h"

enum ErrorCode
{
    NO_ERROR,
    SYNTAX_ERROR,
    REALLOC_ERROR
};

struct Parser
{
    char *str;
    size_t curOffset;
};

struct Lexer
{
    Node_t *token;
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

void LexicalAnalysis(Parser *parser, Lexer *lexer);

#endif // PROGRAMMING_LANGUAGE_H_

