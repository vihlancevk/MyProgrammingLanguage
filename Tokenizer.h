#ifndef PROGRAMMING_LANGUAGE_H_
#define PROGRAMMING_LANGUAGE_H_

#include "Tree.h"

const size_t STR_MAX_SIZE = 20;

enum ErrorCode
{
    NO_ERROR,
    SYNTAX_ERROR,
    REALLOC_ERROR
};

/*enum Keyword
{
    MAIN     ,
    RETURN   ,
    SEMICOLON,
    PRINT    ,
    SCAN     ,
    IF       ,
    ELSE     ,
    WHILE    ,
    ASSIGN   ,
    LSB      ,
    RSB      ,
    LB       ,
    RB       ,
    COMMA    ,
    ADD      ,
    SUB      ,
    MUL      ,
    DIV      ,
    POW      ,
    BAA      ,
    BAB      ,
    BAE      ,
    BAAE     ,
    BABE     ,
    BANE     ,
    OR       ,
    AND      ,
    NOT      ,
    SIN      ,
    COS      ,
    LN
}*/

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

void Tokenizer(char *str, Lexer *lexer);

#endif // PROGRAMMING_LANGUAGE_H_

