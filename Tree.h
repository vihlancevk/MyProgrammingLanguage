#ifndef TREE_H_
#define TREE_H_

#include <fstream>

typedef char elem_t;

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
    DEFINE     , // 3
    FUNCTION   , // 4
    PARAMETR   , // 5
    CALL       , // 6
    MAIN       , // 7
    RETURN     , // 8
    STATEMENT  , // 9
    DECISION   , // 10
    SEMICOLON  , // 11
    PRINT      , // 12
    SCAN       , // 13
    IF         , // 14
    ELSE       , // 15
    WHILE      , // 16
    ASSIGN     , // 17
    LSB        , // 18
    RSB        , // 19
    LB         , // 20
    RB         , // 21
    LAB        , // 22
    RAB        , // 23
    COMMA      , // 24
    ADD        , // 25
    SUB        , // 26
    MUL        , // 27
    DIV        , // 28
    POW        , // 29
    BAA        , // 30
    BAB        , // 31
    BAE        , // 32
    BAAE       , // 33
    BABE       , // 34
    BANE       , // 35
    OR         , // 36
    AND        , // 37
    NOT        , // 38
    LN         , // 39
    SIN        , // 40
    COS          // 41
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

void SetNodeTypeAndValue(Node_t *node, const NodeType nodeType, const double value);

#endif // TREE_H_
