#include <stdlib.h>
#include <assert.h>
#include "GenerateAsmCode.h"

const char *NAME_OUTPUT_FILE = "asm.txt";

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

static void RamCtor(RAM *ram)
{
    assert(ram != nullptr);

    ram->bp = 0;
    ram->cntIfjmp = 0;
    ram->localVar  = (stack_t*)calloc(1, sizeof(stack_t));
    ram->globalVar = (stack_t*)calloc(1, sizeof(stack_t));

    StackCtor(ram->localVar , INITIAL_CAPACITY);
    StackCtor(ram->globalVar, INITIAL_CAPACITY);
}

static void RamDtor(RAM *ram)
{
    assert(ram != nullptr);

    ram->bp = 0;
    ram->cntIfjmp = 0;

    StackDtor(ram->localVar );
    StackDtor(ram->globalVar);

    free(ram->localVar );
    free(ram->globalVar);

    ram->localVar  = nullptr;
    ram->globalVar = nullptr;
}

static void ConvertFunctionNodeInCode(Node_t *node, FILE *code)
{
    assert(node != nullptr);
    assert(code != nullptr);

    if (node->leftChild != nullptr)
    {
        if (node->leftChild->nodeType == MAIN)
        {
            fprintf(code, "MAIN\n");
        }
        else
        {
            // ToDo smth
        }
    }
    if (node->rightChild != nullptr)
    {
        // ToDo smth
    }
}

static void ConvertNodeInCode(Node_t *node, FILE *code)
{
    assert(node != nullptr);
    assert(code != nullptr);

    switch ((int)node->nodeType)
    {
        case (int)FUNCTION : { ConvertFunctionNodeInCode(node, code); break; }
        default            : { break; }
    }

    if (node->leftChild  != nullptr) { ConvertNodeInCode(node->leftChild , code); }
    if (node->rightChild != nullptr) { ConvertNodeInCode(node->rightChild, code); }
}

void GenerateAsmCode(Tree_t *tree)
{
    assert(tree != nullptr);

    FILE *code = fopen(NAME_OUTPUT_FILE, "w");

    if (!FindMain(tree))
    {
        printf("NO_MAIN_DECLARATION\n");
        return;
    }

    RAM ram = {};
    RamCtor(&ram);

    fprintf(code, "CRT\n"
                  "PUSH\n"
                  "POP\n"
                  "CALL MAIN\n"
                  "hlt\n");

    ConvertNodeInCode(tree->root, code);

    RamDtor(&ram);
    fclose(code);
}
