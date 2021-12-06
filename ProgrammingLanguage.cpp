#include <assert.h>
#include <math.h>
#include <string.h>
#include "ProgrammingLanguage.h"

#define IS_LEXICAL_ANALYSIS_ERROR()                         \
    do                                                      \
    {                                                       \
        if (parser->errorCode != NO_ERROR)                  \
        {                                                   \
            return;                                         \
        }                                                   \
    } while(0)

static void GetG    (Parser *parser);
static void GetE    (Parser *parser);
static void GetT    (Parser *parser);
static void GetPower(Parser *parser);
static void GetUnary(Parser *parser);
static void GetP    (Parser *parser);
static void GetN    (Parser *parser);

const size_t NUMBER_UNARY_OPERATIONS = 3;
const size_t EXPANSION_COEFFICIENT   = 2;
const double NO_VALUE = -1.0;

UnaryOperation unaryOperation[NUMBER_UNARY_OPERATIONS] = {{ "sin", 3, sin},
                                                          { "cos", 3, cos},
                                                          { "ln" , 2, log}};

static void ReallocNodeArray(Parser *parser)
{
    assert(parser != nullptr);

    if (parser->curSize >= parser->capacity)
    {
        parser->capacity *= EXPANSION_COEFFICIENT;
        Node_t *node = (Node_t*)realloc((void*)parser->node, sizeof(Node_t) * parser->capacity);
        if (node == nullptr)
        {
            parser->errorCode = REALLOC_ERROR;
            return;
        }
        parser->node = node;
    }
}

static void AddElemInNodeArray(Parser *parser, NodeType nodeType, double value)
{
    assert(parser != nullptr);

    ReallocNodeArray(parser);
    parser->node[parser->curSize].nodeType = nodeType;
    parser->node[parser->curSize].value = value;
    parser->curSize++;
}

static void Require(Parser *parser, const char symbol)
{
    assert(parser != nullptr);

    if (*(parser->str + parser->curOffset) == symbol) parser->curOffset++;
    else parser->errorCode = SYNTAX_ERROR;
}

static void GetG(Parser *parser)
{
    assert(parser != nullptr);

    GetE(parser);
    IS_LEXICAL_ANALYSIS_ERROR();

    Require(parser, '$');

    AddElemInNodeArray(parser, END_STR, NO_VALUE);
}

static void GetE(Parser *parser)
{
    assert(parser != nullptr);

    GetT(parser);
    IS_LEXICAL_ANALYSIS_ERROR();

    while (*(parser->str + parser->curOffset) == '+' || *(parser->str + parser->curOffset) == '-')
    {
        char op = *(parser->str + parser->curOffset);

        AddElemInNodeArray(parser, (NodeType)op, NO_VALUE);

        parser->curOffset++;
        GetT(parser);
        IS_LEXICAL_ANALYSIS_ERROR();
    }
}

static void GetT(Parser *parser)
{
    assert(parser != nullptr);

    GetPower(parser);
    IS_LEXICAL_ANALYSIS_ERROR();

    while (*(parser->str + parser->curOffset) == '*' || *(parser->str + parser->curOffset) == '/')
    {
        char op = *(parser->str + parser->curOffset);

        AddElemInNodeArray(parser, (NodeType)op, NO_VALUE);

        parser->curOffset++;
        GetPower(parser);
        IS_LEXICAL_ANALYSIS_ERROR();
    }
}

static void GetPower(Parser *parser)
{
    assert(parser != nullptr);

    GetP(parser);

    while (*(parser->str + parser->curOffset) == '^')
    {
        AddElemInNodeArray(parser, (NodeType)'^', NO_VALUE);

        parser->curOffset++;
        GetP(parser);
        IS_LEXICAL_ANALYSIS_ERROR();
    }
}

static void GetP(Parser *parser)
{
    assert(parser != nullptr);

    if (*(parser->str + parser->curOffset) == '(')
    {
        AddElemInNodeArray(parser, (NodeType)'(', NO_VALUE);

        parser->curOffset++;
        GetE(parser);
        IS_LEXICAL_ANALYSIS_ERROR();
        Require(parser, ')');

       AddElemInNodeArray(parser, (NodeType)')', NO_VALUE);
    }
    else
    {
        size_t curOffset = parser->curOffset;
        GetUnary(parser);
        if (curOffset == parser->curOffset) { GetN(parser); }
        IS_LEXICAL_ANALYSIS_ERROR();
    }
}

static void GetUnary(Parser *parser)
{
    assert(parser != nullptr);

    for (size_t i = 0; i < NUMBER_UNARY_OPERATIONS; i++)
    {
        if (strncmp(unaryOperation[i].str, (parser->str + parser->curOffset), unaryOperation[i].strSize) == 0)
        {
            char op[unaryOperation[i].strSize] = {};
            strncpy(op, parser->str + parser->curOffset, unaryOperation[i].strSize);

            AddElemInNodeArray(parser, (NodeType)op[0], NO_VALUE);

            parser->curOffset += unaryOperation[i].strSize;
            GetP(parser);
        }
    }
}

static void GetN(Parser *parser)
{
    assert(parser != nullptr);

    double val = 0;
    const char *oldS = parser->str + parser->curOffset;
    while (*(parser->str + parser->curOffset) >= '0' && '9' >= *(parser->str + parser->curOffset))
    {
        val = val * 10 + (*(parser->str + parser->curOffset) - '0');
        parser->curOffset++;
    }
    if (oldS == (parser->str + parser->curOffset)) parser->errorCode = SYNTAX_ERROR;

    AddElemInNodeArray(parser, CONST, val);
}

#undef IS_LEXICAL_ANALYSIS_ERROR

void LexicalAnalysis(Parser *parser)
{
    assert(parser != nullptr);

    GetG(parser);
}
