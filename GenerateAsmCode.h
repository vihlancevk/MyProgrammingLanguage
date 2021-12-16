#ifndef GENERATE_ASM_CODE_H_
#define GENERATE_ASM_CODE_H_

#include "Tree.h"
#include "Stack.h"

const size_t INITIAL_CAPACITY = 20;
const size_t REG_NAME_SIZE    = 2;
const size_t NUMBERS_VARIABLE = 10;
const size_t NUMBERS_LABEL    = 10;

struct Name
{
    char str[STR_MAX_SIZE];
    size_t curOffset;
};

struct TableLocalNames
{
    Name localNames[NUMBERS_VARIABLE];
    size_t curName;
    size_t curOffset;
};

struct TableGlobalNames
{
    Name globalNames[NUMBERS_VARIABLE];
    size_t curName;
    size_t curOffset;
};

void GenerateAsmCode(Tree_t *tree);

#endif // GENERATE_ASM_CODE_H_
