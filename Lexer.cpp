#include <assert.h>
#include <math.h>
#include <string.h>
#include "Lexer.h"

const size_t EXPANSION_COEFFICIENT = 2   ;
const double NO_VALUE              = -1.0;

static void ReallocTokenArray(Lexer *lexer)
{
    assert(lexer != nullptr);

    if (lexer->curToken >= lexer->capacity)
    {
        lexer->capacity *= EXPANSION_COEFFICIENT;
        Token *token = (Token*)realloc((void*)lexer->token, sizeof(Token) * lexer->capacity);
        if (token == nullptr)
        {
            lexer->errorCode = REALLOC_ERROR;
            return;
        }
        lexer->token = token;
    }
}

static void AddElemInTokenArray(Lexer *lexer, char *str)
{
    assert(lexer != nullptr);
    assert(str   != nullptr);

    ReallocTokenArray(lexer);

    lexer->token[lexer->curToken].size = strlen(str);
    lexer->token[lexer->curToken].str = (char*)calloc(lexer->token[lexer->curToken].size, sizeof(char));
    strcpy(lexer->token[lexer->curToken].str, str);

    lexer->curToken++;
}

const char *WHITESPACE_CHARACTERS = " \n\t";

static size_t SkeapWhitespaceCharacters(char *str, size_t curOffset)
{
    assert(str != nullptr);

    while (strchr(WHITESPACE_CHARACTERS, *(str + curOffset)) != nullptr)
    {
        curOffset++;
    }

    return curOffset;
}

void LexicalAnalysis(char *str, Lexer *lexer)
{
    assert(str   != nullptr);
    assert(lexer != nullptr);

    size_t curOffset = 0;

    while(*(str + curOffset) != '\0')
    {
        curOffset = SkeapWhitespaceCharacters(str, curOffset);

        #define KEY_WORD(word, count, replace, nodeType, nameOperation) \
            if (strncmp(str + curOffset, word, count) == 0)             \
            {                                                           \
                AddElemInTokenArray(lexer, replace);                    \
                curOffset += count;                                     \
                continue;                                               \
            }

        #include "KeyWorld.h"

        if (isdigit((int)(unsigned char)*(str + curOffset)))
        {
            char *strPtr = str + curOffset;

            while (isdigit((int)(unsigned char)*(str + curOffset)))
            {
                curOffset++;

            }

            AddElemInTokenArray(lexer, strndup(strPtr, str + curOffset - strPtr));
            continue;
        }

        if (isalpha((int)(unsigned char)*(str + curOffset)))
        {
            char *strPtr = str + curOffset;

            while((isalpha((int)(unsigned char)*(str + curOffset))  ||
                   isdigit((int)(unsigned char)*(str + curOffset))  ||
                   (*(str + curOffset) == '_')))
            {
                curOffset++;
            }

            AddElemInTokenArray(lexer, strndup(strPtr, str + curOffset - strPtr));
            continue;
        }

        if (*(str + curOffset) == '$')
        {
            AddElemInTokenArray(lexer, "$");
            curOffset++;
            continue;
        }

        curOffset = SkeapWhitespaceCharacters(str, curOffset);
    }
}

#undef KEY_WORD
