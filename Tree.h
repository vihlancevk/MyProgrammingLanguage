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
    NUMBER           = 0, // 1, 2, ...
    VARIABLE         = 1, // x, ...
    UNARY_OPERATION  = 2, // sin(), cos(), ln()
    BINARY_OPERATION = 3, // +, -, *, /, ^, =, ==, !=, <, >, <=, >=
    SEPARATOR        = 4, // (, ), {, }, ,
    LR               = 5, // ;
    CON_OPERATOR     = 6, // if()
    CYCLE            = 7, // while()
    FUNC             = 8, // main(), print(), ...
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

void SetNodeTypeAndValue(Node_t *node, const NodeType nodeType,  const double value);

#endif // TREE_H_
