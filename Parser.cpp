#include <assert.h>
#include <math.h>
#include <string.h>
#include "Parser.h"

#define IS_CALCULATOR_ERROR()                       \
    do                                              \
    {                                               \
        if (parser->parserError != PARSER_NO_ERROR) \
        {                                           \
            printf("Syntaxis error\n");             \
            return nullptr;                         \
        }                                           \
    } while(0)

static Node_t* GetG    (Parser *parser);
static Node_t* GetA    (Parser *parser);
static Node_t* GetE    (Parser *parser);
static Node_t* GetT    (Parser *parser);
static Node_t* GetPow  (Parser *parser);
static Node_t* GetUnary(Parser *parser);
static Node_t* GetP    (Parser *parser);
static Node_t* GetN    (Parser *parser);
static Node_t* GetId   (Parser *parser);

const int NUMBER_UNARY_OPERATIONS = 3;
const double NO_VALUE = -1.0;

UnaryOperation unaryOperation[NUMBER_UNARY_OPERATIONS] = {{ SIN, (char*)"sin"},
                                                          { COS, (char*)"cos"},
                                                          { LN , (char*)"lg" }};

static Node_t* NodeCtor(Node_t *node)
{
    assert(node != nullptr);

    Node_t *tNode = (Node_t*)calloc(1, sizeof(Node_t));
    tNode->parent = node->parent;
    tNode->leftChild  = node->leftChild;
    tNode->rightChild = node->rightChild;
    tNode->nodeType = node->nodeType;
    tNode->value = node->value;
    tNode->str = node->str;

    if (node->leftChild  != nullptr) { node->leftChild->parent  = tNode; }
    if (node->rightChild != nullptr) { node->rightChild->parent = tNode; }

    return tNode;
}

static void Require(Parser *parser, const NodeType nodeType)
{
    assert(parser != nullptr);

    if (parser->tokens[parser->curToken].keyword == nodeType) parser->curToken++;
    else parser->parserError = PARSER_SYNTAX_ERROR;
}

static Node_t* GetG(Parser *parser)
{
    assert(parser != nullptr);

    if (parser->tokens[parser->curToken].keyword == MAIN &&
        parser->tokens[parser->curToken + 1].keyword == LB &&
        parser->tokens[parser->curToken + 2].keyword == RB &&
        parser->tokens[parser->curToken + 3].keyword == LSB)
    {
        parser->curToken += 4;

        parser->tree->root->nodeType = STATEMENT;
        parser->tree->root->value = NO_VALUE;
        parser->tree->root->str = (char*)calloc(STR_MAX_SIZE, sizeof(char));
        strcpy(parser->tree->root->str, "statement");

        Node_t *node  = TreeInsert(parser->tree, parser->tree->root, RIGHT_CHILD, DEFINE, NO_VALUE, "define");
        Node_t *node2 = TreeInsert(parser->tree, node, LEFT_CHILD, FUNCTION, NO_VALUE, "function");
        node->rightChild = GetA(parser);
        if (node->rightChild != nullptr) { node->rightChild->parent = node; }
        IS_CALCULATOR_ERROR();

        TreeInsert(parser->tree, node2, LEFT_CHILD, MAIN, NO_VALUE, "main");

        Require(parser, RSB);
    }

    return parser->tree->root;
}

static Node_t* GetA(Parser *parser)
{
    assert(parser != nullptr);

    while (parser->tokens[parser->curToken + 1].keyword == ASSIGN)
    {
        Node_t *node = (Node_t*)calloc(1, sizeof(Node_t));
        Node_t *node2 = TreeInsert(parser->tree, node, RIGHT_CHILD, ASSIGN, NO_VALUE, (char*)"=");
        node2->leftChild = GetId(parser);
        node2->leftChild->parent = node2;

        parser->curToken = parser->curToken + 1;

        TreeInsert(parser->tree, node2, RIGHT_CHILD, INITIALIZER, NO_VALUE, "initializer");
        node2->rightChild->rightChild = GetE(parser);
        IS_CALCULATOR_ERROR();
        node->nodeType = STATEMENT;
        node->value = NO_VALUE;
        node->str = (char*)calloc(STR_MAX_SIZE, sizeof(char));
        strcpy(node->str, "statement");
        Require(parser, SEMICOLON);

        return node;
    }

    return nullptr;
}

static Node_t* GetE(Parser *parser)
{
    assert(parser != nullptr);

    Node_t *val = GetT(parser);
    IS_CALCULATOR_ERROR();

    while (parser->tokens[parser->curToken].keyword == ADD || parser->tokens[parser->curToken].keyword == SUB)
    {
        NodeType op = parser->tokens[parser->curToken].keyword;
        parser->curToken++;
        Node_t *val2 = GetT(parser);
        IS_CALCULATOR_ERROR();
        if (op == ADD)
        {
            Node_t node = {nullptr, val, val2, ADD, NO_VALUE, (char*)"+"};
            val =  NodeCtor(&node);
        }
        else
        {
            Node_t node = {nullptr, val, val2, SUB, NO_VALUE, (char*)"-"};
            val = NodeCtor(&node);
        }
    }

    return val;
}

static Node_t* GetT(Parser *parser)
{
    assert(parser != nullptr);

    Node_t *val = GetPow(parser);
    IS_CALCULATOR_ERROR();

    while (parser->tokens[parser->curToken].keyword == MUL || parser->tokens[parser->curToken].keyword == DIV)
    {
        NodeType op = parser->tokens[parser->curToken].keyword;
        parser->curToken++;
        Node_t *val2 = GetPow(parser);
        IS_CALCULATOR_ERROR();
        if (op == MUL)
        {
            Node_t node = {nullptr, val, val2, MUL, NO_VALUE, (char*)"*"};
            val =  NodeCtor(&node);
        }
        else
        {
            Node_t node = {nullptr, val, val2, DIV, NO_VALUE, (char*)"/"};
            val =  NodeCtor(&node);
        }
    }

    return val;
}

static Node_t* GetPow(Parser *parser)
{
    assert(parser != nullptr);

    Node_t* val = GetP(parser);

    while (parser->tokens[parser->curToken].keyword == POW)
    {
        parser->curToken++;
        Node_t *val2 = GetP(parser);
        IS_CALCULATOR_ERROR();

        Node_t node = {nullptr, val, val2, POW, NO_VALUE, (char*)"^"};

        val =  NodeCtor(&node);
    }

    return val;
}

static Node_t* GetP(Parser *parser)
{
    assert(parser != nullptr);

    Node_t *val = nullptr;

    if (parser->tokens[parser->curToken].keyword == LB)
    {
        parser->curToken++;
        val = GetE(parser);
        IS_CALCULATOR_ERROR();
        Require(parser, RB);
    }
    else
    {
        size_t curToken = parser->curToken;
        val = GetUnary(parser);
        if (curToken == parser->curToken) { val = GetN(parser) ; }
        if (curToken == parser->curToken) { val = GetId(parser); }
        IS_CALCULATOR_ERROR();
    }
    return val;
}

static Node_t* GetUnary(Parser *parser)
{
    assert(parser != nullptr);

    for (int i = 0; i < NUMBER_UNARY_OPERATIONS; i++)
    {
        if (parser->tokens[parser->curToken].keyword == unaryOperation[i].nodeType)
        {
            parser->curToken++;
            Node_t *val = GetP(parser);
            IS_CALCULATOR_ERROR();
            Node_t node = {nullptr, nullptr, val, unaryOperation[i].nodeType, NO_VALUE, unaryOperation[i].str};

            return NodeCtor(&node);
        }
    }

    return nullptr;
}

static Node_t* GetN(Parser *parser)
{
    assert(parser != nullptr);
    if (strcmp(parser->tokens[parser->curToken].id, "\0") == 0)
    {
        Node_t node = {nullptr, nullptr, nullptr, CONST, parser->tokens[parser->curToken].value, nullptr};

        parser->curToken++;

        return NodeCtor(&node);
    }
    else {return nullptr; }
}

static Node_t* GetId(Parser *parser)
{
    assert(parser != nullptr);

    Node_t node = {nullptr, nullptr, nullptr, VARIABLE, NO_VALUE, parser->tokens[parser->curToken].id};

    parser->curToken++;

    return NodeCtor(&node);
}

#undef IS_CALCULATOR_ERROR

Tree_t* SyntacticAnalysis(Parser *parser)
{
    assert(parser != nullptr);

    parser->tree->root = GetG(parser);

    return parser->tree;
}
