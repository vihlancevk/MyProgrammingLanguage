#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "GenerateAsmCode.h"

const char *NAME_OUTPUT_FILE = "asm.txt";

size_t curLabel = 0;

static size_t GenerateLabel()
{
    return curLabel++;
}

static size_t GetLabel()
{
    curLabel--;
    return curLabel++;
}

static int NodeVisitor(Node_t *node)
{
    assert(node != nullptr);

    if (node->nodeType == MAIN) { return 1; }

    if (node->leftChild  != nullptr) { if (NodeVisitor(node->leftChild )) { return 1; } }
    if (node->rightChild != nullptr) { if (NodeVisitor(node->rightChild)) { return 1; } }

    return 0;
}

static int FindMain(Tree_t *tree)
{
    assert(tree != nullptr);

    return NodeVisitor(tree->root);
}

static size_t CheckTableLocalNames(Node_t *node, TableLocalNames *localNames)
{
    assert(node       != nullptr);
    assert(localNames != nullptr);

    size_t curOffset = -1;

    for (size_t i = 0; i < NUMBERS_VARIABLE; i++)
    {
        if (strcmp(node->str, localNames->localNames[i].str) == 0)
        {
            curOffset = localNames->localNames[i].curOffset;
            return curOffset;
        }
    }

    if (localNames->curName <= NUMBERS_VARIABLE)
    {
        strcpy(localNames->localNames[localNames->curName].str, node->str);
        curOffset = localNames->curOffset;
        localNames->localNames[localNames->curName].curOffset = curOffset;
        localNames->curName++;
        localNames->curOffset++;
        return curOffset;
    }
    else
    {
        printf("Invalid number of variables\n");
        return curOffset;
    }
}

static void ConvertSubtreeInCode            (Node_t *node, FILE *code, TableLocalNames *localNames);
static void ConvertDefineNodeInCode         (Node_t *node, FILE *code, TableLocalNames *localNames);
static void ConvertReturnNodeInCode         (Node_t *node, FILE *code, TableLocalNames *localNames);
static void ConvertCallNodeInCode           (Node_t *node, FILE *code, TableLocalNames *localNames);
static void ConvertIfNodeInCode             (Node_t *node, FILE *code, TableLocalNames *localNames);
static void ConvertWhileNodeInCode          (Node_t *node, FILE *code, TableLocalNames *localNames);
static void ConvertAssignNodeInCode         (Node_t *node, FILE *code, TableLocalNames *localNames);
static void ConvertConstNodeInCode          (Node_t *node, FILE *code, TableLocalNames *localNames);
static void ConvertVariableNodeIncode       (Node_t *node, FILE *code, TableLocalNames *localNames);
static void ConvertBinaryOperationNodeInCode(Node_t *node, FILE *code, TableLocalNames *localNames);

static void ConvertDefineNodeInCode(Node_t *node, FILE *code, TableLocalNames *localNames)
{
    assert(node       != nullptr);
    assert(code       != nullptr);

    if (node->leftChild->leftChild->nodeType == MAIN)
    {
        fprintf(code, ":MAIN\n");
    }
    else
    {
        //! ToDo smth
    }

    if (node->leftChild->rightChild != nullptr)
    {
        //! ToDo smth
    }

    TableLocalNames newLocalNames = {};
    newLocalNames.curName = 1;
    newLocalNames.curOffset = 1;

    ConvertSubtreeInCode(node->rightChild, code, &newLocalNames);
}

//! gx - register for return value from function
static void ConvertReturnNodeInCode(Node_t *node, FILE *code, TableLocalNames *localNames)
{
    assert(node       != nullptr);
    assert(code       != nullptr);
    assert(localNames != nullptr);

    if (node->rightChild != nullptr)
    {
        ConvertSubtreeInCode(node->rightChild, code, localNames);

        fprintf(code, "POP gx\n"
                      "RET\n");
    }
    else
    {
        fprintf(code, "RET\n");
    }
}

//! fx - helpful register for sqrt operation
static void ConvertCallNodeInCode(Node_t *node, FILE *code, TableLocalNames *localNames)
{
    assert(node       != nullptr);
    assert(code       != nullptr);
    assert(localNames != nullptr);

    if (node->leftChild->nodeType == SCAN || node->leftChild->nodeType == PRINT || node->leftChild->nodeType == SQRT)
    {
        if (node->leftChild->nodeType == SCAN)       {                                                           fprintf(code, "IN\n")                      ; }
        else if (node->leftChild->nodeType == PRINT) { ConvertSubtreeInCode(node->rightChild, code, localNames); fprintf(code, "OUT\n")                     ; }
        else                                         { ConvertSubtreeInCode(node->rightChild, code, localNames); fprintf(code, "POP fx\nSQRT fx\nPUSH fx\n"); }
    }
    else
    {
        //! ToDo smth
    }
}

static void ConvertIfNodeInCode(Node_t *node, FILE *code, TableLocalNames *localNames)
{
    assert(node       != nullptr);
    assert(code       != nullptr);
    assert(localNames != nullptr);

    size_t label = 0;

    ConvertSubtreeInCode(node->leftChild, code, localNames);
    label = GenerateLabel();
    if (node->rightChild->rightChild != nullptr) { ConvertSubtreeInCode(node->rightChild->rightChild, code, localNames); fprintf(code, "JMP next%zu\n", label); }
    fprintf(code, ":next%zu\n", label - 1);
    ConvertSubtreeInCode(node->rightChild->leftChild, code, localNames);
    fprintf(code, ":next%zu\n", label);
}

static void ConvertWhileNodeInCode(Node_t *node, FILE *code, TableLocalNames *localNames)
{
    assert(node       != nullptr);
    assert(code       != nullptr);
    assert(localNames != nullptr);

    size_t label = GenerateLabel();

    fprintf(code, ":next%zu\n", label);
    ConvertSubtreeInCode(node->leftChild, code, localNames);
    label = GenerateLabel();
    fprintf(code, "JMP next%zu\n", label);
    fprintf(code, ":next%zu\n", label - 1);
    ConvertSubtreeInCode(node->rightChild, code, localNames);
    fprintf(code, "JMP next%zu\n", label - 2);
    fprintf(code, ":next%zu\n", label);
}

static void ConvertAssignNodeInCode(Node_t *node, FILE *code, TableLocalNames *localNames)
{
    assert(node       != nullptr);
    assert(code       != nullptr);
    assert(localNames != nullptr);

    ConvertSubtreeInCode(node->rightChild, code, localNames);
    size_t curOffset = CheckTableLocalNames(node->leftChild, localNames);
    fprintf(code, "POP [%zu]\n", curOffset);
}

static void ConvertConstNodeInCode(Node_t *node, FILE *code, TableLocalNames *localNames)
{
    assert(node       != nullptr);
    assert(code       != nullptr);
    assert(localNames != nullptr);

    fprintf(code, "PUSH %g\n", node->value);
}

static void ConvertVariableNodeIncode(Node_t *node, FILE *code, TableLocalNames *localNames)
{
    assert(node       != nullptr);
    assert(code       != nullptr);
    assert(localNames != nullptr);

    size_t curOffset = CheckTableLocalNames(node, localNames);
    fprintf(code, "PUSH [%zu]\n", curOffset);
}

static void ConvertBinaryOperationNodeInCode(Node_t *node, FILE *code, TableLocalNames *localNames)
{
    assert(node       != nullptr);
    assert(code       != nullptr);
    assert(localNames != nullptr);

    ConvertSubtreeInCode(node->leftChild , code, localNames);
    ConvertSubtreeInCode(node->rightChild, code, localNames);
    switch ((int)node->nodeType)
    {
        case (int)ADD : { fprintf(code, "ADD\n")                          ; break; }
        case (int)SUB : { fprintf(code, "SUB\n")                          ; break; }
        case (int)MUL : { fprintf(code, "MUL\n")                          ; break; }
        case (int)DIV : { fprintf(code, "ADD\n")                          ; break; }
        case (int)POW : { fprintf(code, "ADD\n")                          ; break; }
        case (int)JA  : { fprintf(code, "JA next%zu\n" , GenerateLabel()) ; break; }
        case (int)JB  : { fprintf(code, "JB next%zu\n" , GenerateLabel()) ; break; }
        case (int)JE  : { fprintf(code, "JE next%zu\n" , GenerateLabel()) ; break; }
        case (int)JAE : { fprintf(code, "JAE next%zu\n", GenerateLabel()) ; break; }
        case (int)JBE : { fprintf(code, "JBE next%zu\n", GenerateLabel()) ; break; }
        case (int)JNE : { fprintf(code, "JNE next%zu\n", GenerateLabel()) ; break; }
        default       : { printf("Invalid binary operations\n")           ; break; }
    }
}

static void ConvertSubtreeInCode(Node_t *node, FILE *code, TableLocalNames *localNames)
{
    assert(node       != nullptr);
    assert(code       != nullptr);
    assert(localNames != nullptr);

    switch ((int)node->nodeType)
    {
        case (int)DEFINE   : { ConvertDefineNodeInCode         (node, code, localNames); return; }
        case (int)RETURN   : { ConvertReturnNodeInCode         (node, code, localNames); return; }
        case (int)CALL     : { ConvertCallNodeInCode           (node, code, localNames); return; }
        case (int)IF       : { ConvertIfNodeInCode             (node, code, localNames); return; }
        case (int)WHILE    : { ConvertWhileNodeInCode          (node, code, localNames); return; }
        case (int)ASSIGN   : { ConvertAssignNodeInCode         (node, code, localNames); return; }
        case (int)CONST    : { ConvertConstNodeInCode          (node, code, localNames); return; }
        case (int)VARIABLE : { ConvertVariableNodeIncode       (node, code, localNames); return; }
        case (int)ADD      : { ConvertBinaryOperationNodeInCode(node, code, localNames); return; }
        case (int)SUB      : { ConvertBinaryOperationNodeInCode(node, code, localNames); return; }
        case (int)MUL      : { ConvertBinaryOperationNodeInCode(node, code, localNames); return; }
        case (int)DIV      : { ConvertBinaryOperationNodeInCode(node, code, localNames); return; }
        case (int)POW      : { ConvertBinaryOperationNodeInCode(node, code, localNames); return; }
        case (int)JA       : { ConvertBinaryOperationNodeInCode(node, code, localNames); return; }
        case (int)JB       : { ConvertBinaryOperationNodeInCode(node, code, localNames); return; }
        case (int)JE       : { ConvertBinaryOperationNodeInCode(node, code, localNames); return; }
        case (int)JAE      : { ConvertBinaryOperationNodeInCode(node, code, localNames); return; }
        case (int)JBE      : { ConvertBinaryOperationNodeInCode(node, code, localNames); return; }
        case (int)JNE      : { ConvertBinaryOperationNodeInCode(node, code, localNames); return; }
        default            : { break; }
    }

    if (node->leftChild  != nullptr) { ConvertSubtreeInCode(node->leftChild , code, localNames); }
    if (node->rightChild != nullptr) { ConvertSubtreeInCode(node->rightChild, code, localNames); }
}

void GenerateAsmCode(Tree_t *tree)
{
    assert(tree != nullptr);

    FILE *code = fopen(NAME_OUTPUT_FILE, "w");

    if (!FindMain(tree))
    {
        printf("No main declaration\n");
        return;
    }

    TableLocalNames localNames = {};
    localNames.curName = 1;
    localNames.curOffset = 1;

    fprintf(code, "CRT\n"
                  "CALL MAIN\n"
                  "HLT\n");

    ConvertSubtreeInCode(tree->root, code, &localNames);

    fclose(code);
}
