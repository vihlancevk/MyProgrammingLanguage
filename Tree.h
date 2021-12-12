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
    NO_KEYWORD ,
    MAIN       ,
    RETURN     ,
    SEMICOLON  ,
    PRINT      ,
    PRINTF     ,
    SCAN       ,
    IF         ,
    WHILE      ,
    ASSIGN     ,
    LSB        ,
    RSB        ,
    LB         ,
    RB         ,
    COMMA      ,
    ADD        ,
    SUB        ,
    MUL        ,
    DIV        ,
    POW        ,
    BAA        ,
    BAB        ,
    BAE        ,
    BAAE       ,
    BABE       ,
    BANE       ,
    LN         ,
    SIN        ,
    COS        ,
    TG         ,
    CTG        ,
    ARCSIN     ,
    ARCCOS     ,
    ARCTG      ,
    ARCCTG     ,
    SH         ,
    CH         ,
    TH         ,
    CTH        ,
    SQRT       ,
    EXP
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

void NodeDtor(Node_t *node);

void SubtreeDtor(Node_t *node);

TreeErrorCode TreeDtor(Tree_t *tree);

void SetNodeTypeAndValue(Node_t *node, const NodeType nodeType, const double value);

#endif // TREE_H_
