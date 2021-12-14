#include <assert.h>
#include <math.h>
#include <string.h>
#include "Parser.h"

#define IS_PARSER_ERROR()                           \
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
static Node_t* GetIf   (Parser *parser);
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

static Node_t* NodeCtor(Node_t *thisNode)
{
    assert(thisNode != nullptr);

    Node_t *node = (Node_t*)calloc(1, sizeof(Node_t));
    node->parent = thisNode->parent;
    node->leftChild  = thisNode->leftChild;
    node->rightChild = thisNode->rightChild;
    node->nodeType = thisNode->nodeType;
    node->value = thisNode->value;
    if (thisNode->str != nullptr)
    {
        node->str = (char*)calloc(STR_MAX_SIZE, sizeof(char));
        strcpy(node->str, thisNode->str);
    }
    else
    {
        node->str = thisNode->str;
    }

    if (thisNode->leftChild  != nullptr) { thisNode->leftChild->parent  = node; }
    if (thisNode->rightChild != nullptr) { thisNode->rightChild->parent = node; }

    return node;
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
        IS_PARSER_ERROR();
        if (node->rightChild != nullptr) { node->rightChild->parent = node; }

        TreeInsert(parser->tree, node2, LEFT_CHILD, MAIN, NO_VALUE, "main");

        Require(parser, RSB);
    }

    return parser->tree->root;

}

static Node_t* GetA(Parser *parser)
{
    assert(parser != nullptr);

    Node_t *node = nullptr;

    if (parser->tokens[parser->curToken + 1].keyword == ASSIGN)
    {
        while (parser->tokens[parser->curToken + 1].keyword == ASSIGN)
        {
            Node_t *node1 = (Node_t*)calloc(1, sizeof(Node_t));
            Node_t *node2 = TreeInsert(parser->tree, node1, RIGHT_CHILD, ASSIGN, NO_VALUE, (char*)"=");
            node2->leftChild = GetId(parser);
            IS_PARSER_ERROR();
            node2->leftChild->parent = node2;

            parser->curToken = parser->curToken + 1;

            node2->rightChild = GetE(parser);
            IS_PARSER_ERROR();
            node2->rightChild->parent = node2;
            node1->nodeType = STATEMENT;
            node1->value = NO_VALUE;
            node1->str = (char*)calloc(STR_MAX_SIZE, sizeof(char));
            strcpy(node1->str, "statement");
            Require(parser, SEMICOLON);

            if (node != nullptr)
            {
                node1->leftChild = node;
                node->parent = node1;
                node = node1;
            }
            else
            {
                node = node1;
            }
        }
    }

    Node_t *node1 = GetIf(parser);
    IS_PARSER_ERROR();

    if (node1 != nullptr)
    {
        if (node != nullptr)
        {
            node1->leftChild = node;
            node->parent = node1;
            node = node1;
        }
        else
        {
            node = node1;
        }
    }

    return node;
}

static Node_t* GetIf(Parser *parser)
{
    assert(parser != nullptr);

    Node_t *node = nullptr;

    if (parser->tokens[parser->curToken].keyword == IF && parser->tokens[parser->curToken + 1].keyword == LB)
    {
        parser->curToken = parser->curToken + 2;

        Node_t *node1 = (Node_t*)calloc(1, sizeof(Node_t));
        Node_t *node2 = TreeInsert(parser->tree, node1, RIGHT_CHILD, IF, NO_VALUE, (char*)"if");
        node2->leftChild = GetE(parser);
        IS_PARSER_ERROR();
        node2->leftChild->parent = node2;
        Require(parser, RB);

        Node_t *node3 = TreeInsert(parser->tree, node2, RIGHT_CHILD, DECISION, NO_VALUE, (char*)"decision");

        if (parser->tokens[parser->curToken].keyword == LSB)
        {
            parser->curToken++;

            node3->leftChild = GetA(parser);
            IS_PARSER_ERROR();
            node3->leftChild->parent = node3;
            Require(parser, RSB);
        }

        if (parser->tokens[parser->curToken].keyword == ELSE && parser->tokens[parser->curToken + 1].keyword == LSB)
        {
            parser->curToken = parser->curToken + 2;

            node3->rightChild = GetA(parser);
            IS_PARSER_ERROR();
            node3->rightChild->parent = node3;
            Require(parser, RSB);
        }

        node1->nodeType = STATEMENT;
        node1->value = NO_VALUE;
        node1->str = (char*)calloc(STR_MAX_SIZE, sizeof(char));
        strcpy(node1->str, (char*)"statement");

        node = node1;
    }

    return node;
}

static Node_t* GetE(Parser *parser)
{
    assert(parser != nullptr);

    Node_t *val = GetT(parser);
    IS_PARSER_ERROR();

    while (parser->tokens[parser->curToken].keyword == ADD || parser->tokens[parser->curToken].keyword == SUB)
    {
        NodeType op = parser->tokens[parser->curToken].keyword;
        parser->curToken++;
        Node_t *val2 = GetT(parser);
        IS_PARSER_ERROR();
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
    IS_PARSER_ERROR();

    while (parser->tokens[parser->curToken].keyword == MUL || parser->tokens[parser->curToken].keyword == DIV)
    {
        NodeType op = parser->tokens[parser->curToken].keyword;
        parser->curToken++;
        Node_t *val2 = GetPow(parser);
        IS_PARSER_ERROR();
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
        IS_PARSER_ERROR();

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
        IS_PARSER_ERROR();
        Require(parser, RB);
    }
    else
    {
        size_t curToken = parser->curToken;
        val = GetUnary(parser);
        if (curToken == parser->curToken) { val = GetN(parser) ; }
        if (curToken == parser->curToken) { val = GetId(parser); }
        IS_PARSER_ERROR();
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
            IS_PARSER_ERROR();
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

#undef IS_PARSER_ERROR

Tree_t* SyntacticAnalysis(Parser *parser)
{
    assert(parser != nullptr);

    parser->tree->root = GetG(parser);

    return parser->tree;
}
