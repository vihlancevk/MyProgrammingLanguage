#ifndef TREE_H_
#define TREE_H_

#include <fstream>

typedef char elem_t;

const size_t STR_MAX_SIZE = 400;

enum NodeChild
{
    RIGHT_CHILD,
    LEFT_CHILD
};

enum TreeStatus
{
    TREE_NOT_CONSTRUCTED,
    TREE_CONSTRUCTED,
    TREE_DESTRUCTED
};

enum TreeErrorCode
{
    TREE_NO_ERROR,
    TREE_CONSTRUCTED_ERROR,
    TREE_INSERT_ERROR,
    TREE_DESTRUCTED_ERROR,
    TREE_FILL_ERROR,
    TREE_OBJECT_DEFINITION_MODE_ERROR
};

enum NodeType
{
    CONST      , // 0
    VARIABLE   , // 1
    FUNC       , // 2
    STATEMENT  , // 3
    DEFINE     , // 4
    FUNCTION   , // 5
    PARAMETR   , // 6
    CALL       , // 7
    MAIN       , // 8
    SCAN       , // 9
    PRINT      , // 10
    SQRT       , // 11
    RETURN     , // 12
    IF         , // 13
    ELSE       , // 14
    WHILE      , // 15
    DECISION   , // 16
    ASSIGN     , // 17
    LSB        , // 18
    RSB        , // 19
    LB         , // 20
    RB         , // 21
    LAB        , // 22
    RAB        , // 23
    COMMA      , // 24
    SEMICOLON  , // 25
    ADD        , // 26
    SUB        , // 27
    MUL        , // 28
    DIV        , // 29
    POW        , // 30
    JA         , // 31
    JB         , // 32
    JE         , // 33
    JAE        , // 34
    JBE        , // 35
    JNE          // 36
};

struct Node_t
{
    Node_t *parent;
    Node_t *leftChild;
    Node_t *rightChild;
    NodeType nodeType;
    double value;
    char *str;
};

struct Tree_t
{
    Node_t *root;
    size_t size;
    TreeStatus status;
};

void TreeDump(Tree_t *tree);

TreeErrorCode TreeCtor(Tree_t *tree);

Node_t* TreeInsert(Tree_t *tree, Node_t *node, const NodeChild child, const NodeType tNodeType, const double tValue, const char *tStr);

void SubtreeDtor(Node_t *node);

TreeErrorCode TreeDtor(Tree_t *tree);

void SetNodeTypeValueStr(Node_t *node, const NodeType nodeType, const double value, char *str);

#endif // TREE_H_
