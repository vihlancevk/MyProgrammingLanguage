#include <stdio.h>
#include <stdlib.h>
#include "../include/FileOperations.h"
#include "../include/Stack.h"

const char *INPUT_FILE  = "CPU/res/asm.txt";
const char *OUTPUT_FILE = "CPU/res/code.txt";
const char *LISTING     = "CPU/Assembler/lst.txt";
const int SIZE_ARRAY_LABEL_INFO = 20;
const int MAX_SIZE_STR = 20;
const int NUMBER_REG = 7;
const int FIELD_WIDTH = 31;

enum Commands
{
    #define DEF_CMD_(num, name, arg, code) \
        CPU_##name = (num),

    #include "../include/Commands.h"
    #undef COMMANDS_H_
    #undef DEF_CMD_
};

const uint16_t OPERATION_USE_MEM = 0b1000'0000;
const uint16_t OPERATION_USE_REG = 0b0100'0000;
const uint16_t OPERATION_USE_IMM = 0b0010'0000;
const uint16_t OPERATION_COMMAND = 0b0001'1111;

enum SpecialCodeComands
{
    RAM_REG_PUSH = OPERATION_USE_MEM | OPERATION_USE_REG | 0b11,
    RAM_IMM_PUSH = OPERATION_USE_MEM | OPERATION_USE_IMM | 0b11,
    REG_PUSH = OPERATION_USE_REG | 0b11,
    IMM_PUSH = OPERATION_USE_IMM | 0b11,
    RAM_REG_POP = OPERATION_USE_MEM | OPERATION_USE_REG | 0b1010,
    RAM_IMM_POP = OPERATION_USE_MEM | OPERATION_USE_IMM | 0b1010,
    REG_POP = OPERATION_USE_REG | 0b1010,
    RAM_REG_SQRT = OPERATION_USE_MEM | OPERATION_USE_REG | 0b10100,
    RAM_IMM_SQRT = OPERATION_USE_MEM | OPERATION_USE_IMM | 0b10100,
    REG_SQRT = OPERATION_USE_REG | 0b10100,
};

enum StatusAssembling
{
    ASSEMBLING_SUCCESSFUL,
    ASSEMBLING_WRONG
};

struct LabelInfo
{
    char str[SIZE_ARRAY_LABEL_INFO];
    stackData_t value;
};

struct Assembler
{
    FILE *foutput;
    FILE *flst;
    LabelInfo *labelInfo;
    char nameOperation[MAX_SIZE_STR];
    stackData_t elem;
    short int codeOperation;
    size_t numberBytesFile;
};

const uint64_t SIG = 'DED!VKST';
const int VER = 3;

struct Hdr
{
    uint64_t sig;
    int ver;
};

void *fillStructLine(const char *nameFile,int *linesCount);
void discardComments(Line *lines, int linesCount);
int defineRegistr(char *str);
StatusAssembling processCmdWithArg(Line *lines, int curLineNum, int cnt, Assembler *assembler, char *name);
StatusAssembling processCmdWithoutArg(struct Assembler *assembler, char *name);
StatusAssembling processLabel(Assembler *assembler, size_t *numberLabelInfo);
StatusAssembling assemblerFile(Line *lines, int linesCount, const char *nameOutputFile, Hdr *hdr, LabelInfo *labelInfo);

int main()
{
    Hdr hdr = {SIG, VER};
    int linesCount = 0;
    Line *lines = (Line *)fillStructLine(INPUT_FILE, &linesCount);
    discardComments(lines, linesCount);
    LabelInfo labelInfo[SIZE_ARRAY_LABEL_INFO] = {};
    int i = 0;
    for (i = 0; i < 2; i++)
    {
        if (assemblerFile(lines, linesCount, OUTPUT_FILE, &hdr, labelInfo) != ASSEMBLING_SUCCESSFUL)
        {
            return ASSEMBLING_WRONG;
        }
    }

    #ifdef LOG_ENABLED
        for (i = 0; i < SIZE_ARRAY_LABEL_INFO; i++)
        {
            printf("%2d %5s %3lf\n", i, labelInfo[i].str, labelInfo[i].value);
        }
    #endif // LOG_ENABLED

    return ASSEMBLING_SUCCESSFUL;
}

void *fillStructLine(const char* nameFile ,int *linesCount)
{
    assert(nameFile != nullptr);
    assert(linesCount != nullptr);

    FILE *finput = fopen(nameFile, "r");
    assert(finput != nullptr);

    int numberBytesFile = getFileSize(finput);
    if (numberBytesFile == -1)
    {
        printf("Error getFileSize\n");
        return 0;
    }

    char *str = (char*)calloc(numberBytesFile + 1, sizeof(char));
    assert(str != nullptr);

    str  = (char*)readFile(finput, str, numberBytesFile);
    if (str == nullptr)
    {
        printf("Error readFile\n");
        return 0;
    }

    *linesCount = countNumberLines(str, numberBytesFile);

    Line *lines = (Line*)calloc(*linesCount, sizeof(Line));
    assert(lines != nullptr);

    splitToLines(lines, *linesCount, str);

    fclose(finput);

    return lines;
}

void discardComments(Line *lines, int linesCount)
{
    assert(lines != nullptr);
    assert(linesCount > 0);

    char *ptr = nullptr;
    for (int i = 0; i < linesCount; i++)
    {
        ptr = strchr(lines[i].str, ';');
        if (ptr != nullptr)
        {
            *ptr = '\0';
        }
    }
}

const char* REGISTERS_IDS[NUMBER_REG] = {"ax", "bx", "cx", "dx", "ex", "fx", "gx"};

int defineRegistr(char *str)
{
    assert(str != nullptr);

    for (int i = 0; i < NUMBER_REG; i++)
    {
        if (strcmp(str, REGISTERS_IDS[i]) == 0)
            return (i + 1);
    }

    return 0;
}

static stackData_t checkLabelJump(LabelInfo *labelInfo, char *str) // strcmp
{
    assert(labelInfo != nullptr);
    assert(str != nullptr);

    for (int i = 0; i < SIZE_ARRAY_LABEL_INFO; i++)
    {
        if (strcmp(labelInfo[i].str, str) == 0)
        {
            return labelInfo[i].value;
        }
    }

    return -1;
}

StatusAssembling processCmdWithArg(Line *lines, int curLineNum, int cnt, Assembler *assembler, char *name)
{
    assert(lines != nullptr);
    assert(assembler != nullptr);
    assert(name != nullptr);

    char str[MAX_SIZE_STR] = {};
    char nameRegistr[MAX_SIZE_STR] = {};
    char numberRegister = 0;

    if (cnt == 1)
    {
        sscanf(lines[curLineNum].str, "%s %s", assembler->nameOperation, str);
        if (str[0] == '[')
        {
            if (str[2] == 'x' && str[3] == ']')
            {
                memcpy(nameRegistr, str + 1, 2);
                numberRegister = defineRegistr(nameRegistr);

                #ifdef LOG_ENABLED
                    printf("nameRegistr : %s - numberReistr : %d\n",nameRegistr, numberRegister);
                #endif // LOG_ENABLED

                if (strcmp(name, "PUSH") == 0)
                {
                    assembler->codeOperation = (short int)RAM_REG_PUSH;
                }
                else if (strcmp(name, "POP") == 0)
                {
                    assembler->codeOperation = (short int)RAM_REG_POP;
                }
                else
                {
                    assembler->codeOperation = (short int)RAM_REG_SQRT;
                }
                fwrite(&assembler->codeOperation, sizeof(short int), 1, assembler->foutput);
                fwrite(&numberRegister, sizeof(char), 1, assembler->foutput);
                assembler->numberBytesFile += sizeof(short int) + sizeof(char);
                fprintf(assembler->flst, "%-5s %-5x %x\n", name, assembler->codeOperation, numberRegister);
            }
            else
            {
                for (size_t i = 0; i < strlen(str); i++)
                {
                    str[i] = str[i + 1];
                }
                assembler->elem = atof(str);
                if (strcmp(name, "PUSH") == 0)
                {
                    assembler->codeOperation = (short int)RAM_IMM_PUSH;
                }
                else if (strcmp(name, "POP") == 0)
                {
                    assembler->codeOperation = (short int)RAM_IMM_POP;
                }
                else
                {
                    assembler->codeOperation = (short int)RAM_IMM_SQRT;
                }
                fwrite(&assembler->codeOperation, sizeof(short int), 1, assembler->foutput);
                fwrite(&assembler->elem, sizeof(stackData_t), 1, assembler->foutput);
                assembler->numberBytesFile += sizeof(short int) + sizeof(stackData_t);
                fprintf(assembler->flst, "%-5s %-5x %lf\n", name, assembler->codeOperation, assembler->elem);
            }
        }
        else if (assembler->nameOperation[0] == 'J' || assembler->nameOperation[0] == 'C')
        {
            assembler->elem = checkLabelJump(assembler->labelInfo, str);

            #ifdef LOG_ENABLED
                printf("label : %d\n", assembler->elem);
            #endif // LOG_ENABLED

            fwrite(&assembler->codeOperation, sizeof(short int), 1, assembler->foutput);
            fwrite(&assembler->elem, sizeof(stackData_t), 1, assembler->foutput);
            assembler->numberBytesFile += sizeof(short int) + sizeof(stackData_t);
            fprintf(assembler->flst, "%-5s %-5x %lf\n", name, assembler->codeOperation, assembler->elem);
        }
        else
        {
            numberRegister = defineRegistr(str);
            if (numberRegister == 0)
            {
                return ASSEMBLING_WRONG;
            }
            if (strcmp(name, "PUSH") == 0)
            {
                assembler->codeOperation = (short int)REG_PUSH;
            }
            else if (strcmp(name, "POP") == 0)
            {
                assembler->codeOperation = (short int)REG_POP;
            }
            else
            {
                assembler->codeOperation = (short int)REG_SQRT;
            }
            fwrite(&assembler->codeOperation, sizeof(short int), 1, assembler->foutput);
            fwrite(&numberRegister, sizeof(char), 1, assembler->foutput);
            assembler->numberBytesFile += sizeof(short int) + sizeof(char);
            fprintf(assembler->flst, "%-5s %-5x %x\n", name, assembler->codeOperation, numberRegister);
        }
    }
    else
    {
        assembler->codeOperation = (short int)IMM_PUSH;
        fwrite(&assembler->codeOperation, sizeof(short int), 1, assembler->foutput);
        fwrite(&assembler->elem, sizeof(stackData_t), 1, assembler->foutput);
        assembler->numberBytesFile += sizeof(short int) + sizeof(stackData_t);
        fprintf(assembler->flst, "%-5s %-5x %lf\n", name, assembler->codeOperation, assembler->elem);
    }

    return ASSEMBLING_SUCCESSFUL;
}

StatusAssembling processCmdWithoutArg(struct Assembler *assembler, char *name)
{
    assert(assembler != nullptr);
    assert(name != nullptr);

    fwrite(&assembler->codeOperation, sizeof(short int), 1, assembler->foutput);
    assembler->numberBytesFile += sizeof(short int);
    fprintf(assembler->flst, "%-5s %-5x\n", name, assembler->codeOperation);

    return ASSEMBLING_SUCCESSFUL;
}

StatusAssembling processLabel(Assembler *assembler, size_t *numberLabelInfo)
{
    assert(assembler != nullptr);
    assert(numberLabelInfo != nullptr);

    for (size_t i = 0; i < strlen(assembler->nameOperation); i++) // memcpy or strcpy
    {
        assembler->nameOperation[i] = assembler->nameOperation[i+1];
    }
    strcpy(assembler->labelInfo[*numberLabelInfo].str, assembler->nameOperation);
    assembler->labelInfo[(*numberLabelInfo)++].value = assembler->numberBytesFile;

    #ifdef LOG_ENABLED
        printf("\nlabelInfo[numberLabelInfo++].value : %lf\n\n", assembler->labelInfo[*numberLabelInfo - 1].value);
    #endif // LOG_ENABLED

    return ASSEMBLING_SUCCESSFUL;
}

StatusAssembling assemblerFile(Line *lines, int linesCount, const char *nameOutputFile, Hdr *hdr, LabelInfo *labelInfo)
{
    assert(lines != nullptr);
    assert(linesCount > 0);
    assert(nameOutputFile != nullptr);
    assert(hdr != nullptr);
    assert(labelInfo != nullptr);

    FILE *foutput = fopen(nameOutputFile, "wb");
    FILE *flst = fopen(LISTING, "w");
    assert(foutput != nullptr);
    assert(foutput != nullptr);

    fwrite(hdr, sizeof(Hdr), 1, foutput);
    fprintf(flst, "sig = %zu\nver = %d\n", SIG, VER);

    Assembler assembler = {};
    assembler.foutput = foutput;
    assembler.flst = flst;
    assembler.labelInfo = labelInfo;
    int cnt = 0;
    size_t numberLabelInfo = 0;

    for (int curLineNum = 0; curLineNum < linesCount; curLineNum++)
    {
        cnt = sscanf(lines[curLineNum].str, "%s %lf", assembler.nameOperation, &assembler.elem);

        #ifdef LOG_ENABLED
            printf("\nnumnberBytesFile : %d\n\n", assembler.numberBytesFile);
            printf("%d - %s\n", curLineNum, lines[curLineNum].str);
        #endif // LOG_ENABLED

        #define DEF_CMD_(num, name, isArg, code)                                                                      \
            if (strcmp(#name, assembler.nameOperation) == 0)                                                          \
            {                                                                                                         \
                assembler.codeOperation = (short int)num;                                                             \
                if (isArg)                                                                                            \
                {                                                                                                     \
                    if (processCmdWithArg(lines, curLineNum, cnt, &assembler, (char*)#name) != ASSEMBLING_SUCCESSFUL) \
                    {                                                                                                 \
                        return ASSEMBLING_WRONG;                                                                      \
                    }                                                                                                 \
                }                                                                                                     \
                else                                                                                                  \
                {                                                                                                     \
                    if (processCmdWithoutArg(&assembler, (char*)#name) != ASSEMBLING_SUCCESSFUL)                      \
                    {                                                                                                 \
                        return ASSEMBLING_WRONG;                                                                      \
                    }                                                                                                 \
                }                                                                                                     \
            }                                                                                                         \
            else if (assembler.nameOperation[0] == ':')                                                               \
            {                                                                                                         \
                if (processLabel(&assembler, &numberLabelInfo) != ASSEMBLING_SUCCESSFUL)                              \
                {                                                                                                     \
                    return ASSEMBLING_WRONG;                                                                          \
                }                                                                                                     \
            }                                                                                                         \
            else

        #include "../include/Commands.h"
        #undef COMMANDS_H_
        #undef DEF_CMD_

        {
            #ifdef LOG_ENABLED
                printf("!ERROR\n");
            #endif // LOG_ENABLED

            fclose(foutput);
            fclose(flst);
            return ASSEMBLING_WRONG;
        }
    }

    fclose(foutput);
    fclose(flst);

    return ASSEMBLING_SUCCESSFUL;
}
