#ifndef GENERATE_ASM_CODE_H_
#define GENERATE_ASM_CODE_H_

#include "Tree.h"
#include "Stack.h"

const size_t INITIAL_CAPACITY = 20;

struct RAM
{
    size_t bp;

    size_t cntIfjmp;

    stack_t *localVar;
    stack_t *globalVar;
};

void GenerateAsmCode(Tree_t *tree);

#endif // GENERATE_ASM_CODE_H_
