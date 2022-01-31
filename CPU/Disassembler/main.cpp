//!TODO create this file.

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../include/FileOperations.h"

enum Commands
{
    #define DEF_CMD_(num, name, isArg, code) \
    CPU_##name = num,

    #include"../include/Commands.h"
    #undef COMMANDS_H_
};
#undef DEF_CMD_

enum DisassemblerErrorCode
{
    DISASSEMBLER_NO_ERROR,
};

const char *INPUT_FILE  = "";
const char *OUTPUT_FILE = "";

DisassemblerErrorCode decompilerFile(int numberBytesFile, char *strBuffer);

int main()
{

    return DISASSEMBLER_NO_ERROR;
}

DisassemblerErrorCode decompilerFile(int numberBytesFile, char *strBuffer)
{

    return DISASSEMBLER_NO_ERROR;
}
