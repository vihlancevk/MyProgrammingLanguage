#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "GenerateAsmCode.h"

const char *NAME_OUTPUT_FILE = "asm.txt";
const int NO_VARIABLE_IN_TABLE_NAME = -1;
const int NO_FUNCTION_IN_TABLE_NAME = -1;

TableGlobalNames globalNames = {{}, 0, 1};
TableFunctions functions = {};

int curLabel = 0;

static int GenerateLabel()
{
    return curLabel++;
}

static int NodeVisitorForSearchMain(Node_t *node)
{
    assert(node != nullptr);

    if (node->nodeType == MAIN) { return 1; }

    if (node->leftChild  != nullptr) { if (NodeVisitorForSearchMain(node->leftChild )) { return 1; } }
    if (node->rightChild != nullptr) { if (NodeVisitorForSearchMain(node->rightChild)) { return 1; } }

    return 0;
}

static int FindMain(Tree_t *tree)
{
    assert(tree != nullptr);

    return NodeVisitorForSearchMain(tree->root);
}

static void NodeVisitorForFindAndPrintGlobalVar(Tree_t *tree, Node_t *node, FILE *code)
{
    assert(tree != nullptr);
    assert(node != nullptr);
    assert(code != nullptr);

    Node_t *ptrNode = nullptr;

    if (node->leftChild != nullptr)
    {
        if (node->leftChild->nodeType == STATEMENT && node->leftChild->rightChild->nodeType == ASSIGN)
        {
            strcpy(globalNames.globalNames[globalNames.curName].str, node->leftChild->rightChild->leftChild->str);
            globalNames.globalNames[globalNames.curName].curOffset = globalNames.curOffset;
            if (node->leftChild->rightChild->rightChild->nodeType == CONST)
            {
                fprintf(code, "PUSH %g\n"
                              "POP [%d]\n", node->leftChild->rightChild->rightChild->value, globalNames.curOffset);
            }
            else
            {
                printf("The global variable is not assigned a constant!\n");
                return;
            }
            globalNames.curName++;
            globalNames.curOffset++;
            if (node->leftChild->leftChild != nullptr)
            {
                ptrNode = node->leftChild;
                node->leftChild = ptrNode->leftChild;
                ptrNode->leftChild->parent = node;
                ptrNode->leftChild = nullptr;
                ptrNode->parent = nullptr;
                SubtreeDtor(ptrNode);
                node = node->leftChild;
                NodeVisitorForFindAndPrintGlobalVar(tree, node, code);
            }
            else
            {
                ptrNode = node->leftChild;
                node->leftChild = ptrNode->leftChild;
                ptrNode->parent = nullptr;
                SubtreeDtor(ptrNode);
            }
        }
        else
        {
            NodeVisitorForFindAndPrintGlobalVar(tree, node->leftChild, code);
        }
    }
    else
    {
        return ;
    }
}

//! Only a constant can be assigned to a global variable, otherwise it will lead to an error
static void FindAndPrintGlobalVar(Tree_t *tree, FILE *code)
{
    assert(tree != nullptr);
    assert(code != nullptr);

    Node_t *node = tree->root;

    while (node->nodeType == STATEMENT && node->rightChild->nodeType == ASSIGN)
    {
        if (globalNames.curName < NUMBERS_VARIABLE)
        {
            strcpy(globalNames.globalNames[globalNames.curName].str, node->rightChild->leftChild->str);
            globalNames.globalNames[globalNames.curName].curOffset = globalNames.curOffset;
            if (node->rightChild->rightChild->nodeType == CONST)
            {
                fprintf(code, "PUSH %g\n"
                              "POP [%d]\n", node->rightChild->rightChild->value, globalNames.curOffset);
            }
            else
            {
                printf("The global variable is not assigned a constant!\n");
                return;
            }
            globalNames.curName++;
            globalNames.curOffset++;
            node = node->leftChild;
            if (node->leftChild == nullptr) { break; }
        }
        else
        {
            printf("Invalid number of global variables!\n");
            return;
        }
    }

    tree->root = node;
    tree->root->parent = nullptr;

    NodeVisitorForFindAndPrintGlobalVar(tree, tree->root, code);

    TreeDump(tree);
}

static int CheckTableGlobalNames(Node_t *node)
{
    assert(node != nullptr);

    int curOffset = NO_VARIABLE_IN_TABLE_NAME;

    for (int i = 0; i < NUMBERS_VARIABLE; i++)
    {
        if (strcmp(node->str, globalNames.globalNames[i].str) == 0)
        {
            curOffset = globalNames.globalNames[i].curOffset;
            return curOffset;
        }
    }

    return curOffset;
}

static int CheckTableLocalNames(Node_t *node, TableLocalNames *localNames)
{
    assert(node       != nullptr);
    assert(localNames != nullptr);

    int curOffset = NO_VARIABLE_IN_TABLE_NAME;

    for (int i = 0; i < NUMBERS_VARIABLE; i++)
    {
        if (strcmp(node->str, localNames->localNames[i].str) == 0)
        {
            curOffset = localNames->localNames[i].curOffset;
            return curOffset;
        }
    }

    if (localNames->curName < NUMBERS_VARIABLE)
    {
        strcpy(localNames->localNames[localNames->curName].str, node->str);
        curOffset = localNames->curOffset;
        localNames->localNames[localNames->curName].curOffset = curOffset;
        localNames->curName++;
        localNames->curOffset++;
    }
    else
    {
        printf("Invalid number of local variables!\n");
    }

    return curOffset;
}

static void FillTableFunctions(Node_t *node)
{
    assert(node != nullptr);

    if (node->rightChild->nodeType == DEFINE && node->rightChild->leftChild->leftChild->nodeType != MAIN)
    {
        strcpy(functions.functions[functions.curName].str, node->rightChild->leftChild->leftChild->str);
        functions.functions[functions.curName].curOffset = functions.curName;
        functions.curName++;
    }

    if (node->leftChild != nullptr) { FillTableFunctions(node->leftChild); }
}

static int CheckTableFunctions(Node_t *node)
{
    assert(node != nullptr);

    int curOffset = NO_FUNCTION_IN_TABLE_NAME;

    for (int i = 0; i < NUMBERS_FUNCTION; i++)
    {
        if (strcmp(node->str, functions.functions[i].str) == 0)
        {
            curOffset = functions.functions[i].curOffset;
            return curOffset;
        }
    }

    return curOffset;
}

static void ConvertSubtreeInCode            (Node_t *node, FILE *code, TableLocalNames *localNames);
static void ConvertDefineNodeInCode         (Node_t *node, FILE *code, TableLocalNames *localNames);
static void ConvertReturnNodeInCode         (Node_t *node, FILE *code, TableLocalNames *localNames);
static void ConvertCallNodeInCode           (Node_t *node, FILE *code, TableLocalNames *localNames);
static void ConvertIfNodeInCode             (Node_t *node, FILE *code, TableLocalNames *localNames);
static void ConvertWhileNodeInCode          (Node_t *node, FILE *code, TableLocalNames *localNames);
static void ConvertAssignNodeInCode         (Node_t *node, FILE *code, TableLocalNames *localNames);
static void ConvertConstNodeInCode          (Node_t *node, FILE *code, TableLocalNames *localNames);
static void ConvertLocVariableNodeIncode    (Node_t *node, FILE *code, TableLocalNames *localNames);
static void ConvertBinaryOperationNodeInCode(Node_t *node, FILE *code, TableLocalNames *localNames);

static void ConvertDefineNodeInCode(Node_t *node, FILE *code, TableLocalNames *localNames)
{
    assert(node != nullptr);
    assert(code != nullptr);

    if (node->leftChild->leftChild->nodeType == MAIN)
    {
        fprintf(code, ":MAIN\n");

        TableLocalNames newLocalNames = {{}, 0, localNames->curOffset};
        ConvertSubtreeInCode(node->rightChild, code, &newLocalNames);
        memcpy(localNames, &newLocalNames, sizeof(TableLocalNames));
    }
    else
    {
        if (functions.curName < NUMBERS_FUNCTION)
        {
            fprintf(code, ":%s\n", node->leftChild->leftChild->str);

            TableLocalNames newLocalNames = {{}, 0, localNames->curOffset};
            ConvertSubtreeInCode(node->rightChild, code, &newLocalNames);
        }
        else
        {
            printf("Invalid number of local variables!\n");
            return;
        }
    }

    if (node->leftChild->rightChild != nullptr)
    {
        //! ToDo smth
    }
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
        int isFunctionInTableFunctions = CheckTableFunctions(node->leftChild);

        if (CheckTableFunctions(node->leftChild) != NO_FUNCTION_IN_TABLE_NAME)
        {
            fprintf(code, "CALL %s\n", node->leftChild->str);
        }
        else
        {
            printf("Invalid function call!\n");
            return;
        }
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

static void ConvertLocVariableNodeIncode(Node_t *node, FILE *code, TableLocalNames *localNames)
{
    assert(node       != nullptr);
    assert(code       != nullptr);
    assert(localNames != nullptr);

    int  curOffset = CheckTableGlobalNames(node);
    if (curOffset == NO_VARIABLE_IN_TABLE_NAME)
    {
        curOffset = CheckTableLocalNames(node, localNames);
    }

    fprintf(code, "PUSH [%d]\n", curOffset);
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
        case (int)ADD : { fprintf(code, "ADD\n")                         ; break; }
        case (int)SUB : { fprintf(code, "SUB\n")                         ; break; }
        case (int)MUL : { fprintf(code, "MUL\n")                         ; break; }
        case (int)DIV : { fprintf(code, "ADD\n")                         ; break; }
        case (int)POW : { fprintf(code, "ADD\n")                         ; break; }
        case (int)JA  : { fprintf(code, "JA next%d\n" , GenerateLabel()) ; break; }
        case (int)JB  : { fprintf(code, "JB next%d\n" , GenerateLabel()) ; break; }
        case (int)JE  : { fprintf(code, "JE next%d\n" , GenerateLabel()) ; break; }
        case (int)JAE : { fprintf(code, "JAE next%d\n", GenerateLabel()) ; break; }
        case (int)JBE : { fprintf(code, "JBE next%d\n", GenerateLabel()) ; break; }
        case (int)JNE : { fprintf(code, "JNE next%d\n", GenerateLabel()) ; break; }
        default       : { printf("Invalid binary operations!\n")         ; break; }
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
        case (int)VARIABLE : { ConvertLocVariableNodeIncode    (node, code, localNames); return; }
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
        printf("No main declaration!\n");
        return;
    }

    fprintf(code, "CRT\n");

    FillTableFunctions(tree->root);
    FindAndPrintGlobalVar(tree, code);
    /*for (int i = 0; i < globalNames.curName; i++)
    {
        printf("%s - %d\n", globalNames.globalNames[i].str, globalNames.globalNames[i].curOffset);
    }*/

    TableLocalNames localNames = {{}, 0, globalNames.curOffset};

    fprintf(code, "CALL MAIN\n"
                  "HLT\n");

    ConvertSubtreeInCode(tree->root, code, &localNames);

    fclose(code);
}
