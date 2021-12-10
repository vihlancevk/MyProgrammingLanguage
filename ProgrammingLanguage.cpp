#include <assert.h>
#include <math.h>
#include <string.h>
#include "ProgrammingLanguage.h"

const size_t STR_MAX_SIZE          = 20  ;
const size_t EXPANSION_COEFFICIENT = 2   ;
const double NO_VALUE              = -1.0;

static void ReallocNodeArray(Lexer *lexer)
{
    assert(lexer != nullptr);

    if (lexer->curToken >= lexer->capacity)
    {
        lexer->capacity *= EXPANSION_COEFFICIENT;
        Node_t *token = (Node_t*)realloc((void*)lexer->token, sizeof(Node_t) * lexer->capacity);
        if (token == nullptr)
        {
            lexer->errorCode = REALLOC_ERROR;
            return;
        }
        lexer->token = token;
    }
}

static void AddElemInNodeArray(Lexer *lexer, NodeType nodeType, double value, char *str = nullptr)
{
    assert(lexer != nullptr);

    ReallocNodeArray(lexer);
    lexer->token[lexer->curToken].nodeType = nodeType;
    lexer->token[lexer->curToken].value = value;
    if (str != nullptr)
    {
        lexer->token[lexer->curToken].str = (char*)calloc(STR_MAX_SIZE, sizeof(char));
        strcpy(lexer->token[lexer->curToken].str, str);
    }
    lexer->curToken++;
}

const char *WHITESPACE_CHARACTERS = " \n\t";
int count = 0;
static void SkeapWhitespaceCharacters(Parser *parser)
{
    assert(parser != nullptr);

    while (strchr(WHITESPACE_CHARACTERS, *(parser->str + parser->curOffset)) != nullptr)
    {
        count++;
        parser->curOffset++;
        if (count == 1000)
        {
            break;
        }
    }
}

void LexicalAnalysis(Parser *parser, Lexer *lexer)
{
    while(*(parser->str + parser->curOffset) != '\0')
    {
        SkeapWhitespaceCharacters(parser);

        #define KEY_WORD(word, count, replace, nodeType, nameOperation)     \
            if (strncmp(parser->str + parser->curOffset, word, count) == 0) \
            {                                                               \
                AddElemInNodeArray(lexer, nodeType, NO_VALUE);              \
                continue;                                                   \
            }

        #include "KeyWorld.h"

        #undef KEY_WORD

        if (isdigit((int)(unsigned char)*(parser->str + parser->curOffset)) ||
            ((*(parser->str + parser->curOffset) == '-') && (isdigit((int)(unsigned char)*(parser->str + parser->curOffset + 1)))))
        {
            char *strPtr = parser->str + parser->curOffset;
            double value = strtod(strPtr, &strPtr);

            AddElemInNodeArray(lexer, NUMBER, value);
            continue;
        }

        if (isalpha((int)(unsigned char)*(parser->str + parser->curOffset)))
        {
            char *strPtr = parser->str + parser->curOffset;

            while((isalpha((int)(unsigned char)*(parser->str + parser->curOffset))  ||
                   isdigit((int)(unsigned char)*(parser->str + parser->curOffset))  ||
                   (*(parser->str + parser->curOffset) == '_')) && (*(parser->str + parser->curOffset) != '\0'))
            {
                parser->curOffset++;
            }

            AddElemInNodeArray(lexer, VARIABLE, NO_VALUE, strndup(strPtr, parser->str + parser->curOffset - strPtr));
            continue;
        }

        SkeapWhitespaceCharacters(parser);
        count++;
        if (count == 1000)
        {
            printf("Error;");
            break;
        }
    }
}

static void GetG    (Lexer *lexer);
static void GetE    (Lexer *lexer);
static void GetT    (Lexer *lexer);
static void GetPower(Lexer *lexer);
static void GetUnary(Lexer *lexer);
static void GetP    (Lexer *lexer);
static void GetN    (Lexer *lexer);
