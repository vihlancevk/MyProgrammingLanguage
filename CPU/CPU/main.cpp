#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <cstdlib>
#include "../include/Stack.h"
#include "../include/FileOperations.h"

#define DEBUG
// #undef  DEBUG

#define IS_CPU_ERROR_(stackError)    \
    if(stackError != STACK_NO_ERROR) \
    {                                \
        return CPU_STACK_ERROR;      \
    }

    enum Commands
    {
    #define DEF_CMD_(num, name, arg, code) \
        CPU_##name = num,

        #include "../include/Commands.h"
        #undef COMMANDS_H_
        #undef DEF_CMD_
    };

const uint64_t SIG = 'DED!VKST';
const int VER = 3;
const uint16_t OPERATION_USE_MEM = 0b1000'0000;
const uint16_t OPERATION_USE_IMM = 0b0010'0000;
const size_t SIZE_MEM = 1000;
const size_t NUMBER_REG = 7;
const double PRECISION = 0.0;
const int DEPTH_RECURSION = 20;
const char *INPUT_FILE  = "CPU/res/code.txt";
const char *OUTPUT_FILE = ".txt";
const int FIELD_WIDTH = 31;

struct CPU_t
{
    stack_t stack = {};
    size_t callArray[DEPTH_RECURSION] = {};
    int cntNumAddress;
    int numberBytesFile;
    char *program;
    char *programCounter;
    stackData_t registers[NUMBER_REG];
    stackData_t *dynMem;
};

enum CPU_ErrorCode
{
    CPU_NO_ERROR,
    CPU_PROGRAM_CALLOC_ERROR,
    CPU_PROGRAM_FILL_ERROR,
    CPU_SIG_ERROR,
    CPU_VER_ERROR,
    CPU_STACK_ERROR,
    CPU_NO_COMMAND,
    CPU_NO_NUMBER_REGISTER,
    CPU_NO_NUMBER_DYN_MEM
};

CPU_ErrorCode fillStructCPU(CPU_t *CPU);
CPU_ErrorCode checkHdr(CPU_t *CPU);
char calculatedCMD(char codeOperation);
stackData_t *getArg(short int codeOperation, struct CPU_t *cpu, CPU_ErrorCode *cpuError);
CPU_ErrorCode executeCode(CPU_t *cpu);
void dumpCPU(CPU_t *CPU);

int main()
{
    #ifdef DEBUG
        // ClearLogFile();
    #endif // DEBUG

    #ifdef LOG_ENABLED
        printf("%d\n\n", sizeof(stackData_t));
    #endif // LOG_ENABLED

    CPU_t cpu = {};
    CPU_ErrorCode cpuError = fillStructCPU(&cpu);
    if (cpuError != CPU_NO_ERROR)
    {
        return cpuError;
    }

    #ifdef LOG_ENABLED
        dumpCPU(&cpu);
    #endif // LOG_ENABLED

    cpuError = executeCode(&cpu);
    
    #ifdef LOG_ENABLED
        int i;
        printf("\na : %lf\nb : %lf\nc : %lf\ncntNumAdress : %d\n\n", cpu.registers[0], cpu.registers[1], cpu.registers[2], cpu.cntNumAddress);
        for (i = 0; i < DEPTH_RECURSION; i++)
        {
            printf("\ni : %d - %x\n", i, cpu.callArray[i]);
        }
        printf("\n");
    #endif // LOG_ENABLED

    return cpuError;
}

CPU_ErrorCode fillStructCPU(CPU_t *cpu)
{
    FILE *finput = fopen(INPUT_FILE, "rb");
    int numberBytesFile = getFileSize(finput);
    cpu->cntNumAddress = 0;
    cpu->numberBytesFile = numberBytesFile;
    cpu->program = (char*)calloc(numberBytesFile + 1, sizeof(char));
    if (cpu->program == nullptr)
    {
        fclose(finput);
        return CPU_PROGRAM_CALLOC_ERROR;
    }

    cpu->program = (char*)readFile(finput, cpu->program, numberBytesFile);
    if (cpu->program == nullptr)
    {
        fclose(finput);
        return CPU_PROGRAM_FILL_ERROR;
    }

    cpu->programCounter = cpu->program;
    cpu->dynMem = (stackData_t*)calloc(SIZE_MEM, sizeof(size_t));
    if (cpu->dynMem == nullptr)
    {
        fclose(finput);
        return CPU_PROGRAM_CALLOC_ERROR;
    }

    CPU_ErrorCode cpuError = checkHdr(cpu);
    if (cpuError != CPU_NO_ERROR)
    {
        fclose(finput);
        return cpuError;
    }

    fclose(finput);
    return CPU_NO_ERROR;
}

CPU_ErrorCode checkHdr(CPU_t *cpu)
{
    assert(cpu != nullptr);

    if (*(uint64_t*)(cpu->programCounter) != SIG)
    {
        return CPU_SIG_ERROR;
    }

    cpu->program = (char*)((char*)cpu->programCounter + sizeof(uint64_t));
    cpu->programCounter = cpu->program;
    cpu->numberBytesFile -= (int)sizeof(uint64_t);

    if (*(int*)(cpu->programCounter) != VER)
    {
        return CPU_VER_ERROR;
    }

    cpu->program = (char*)((char*)cpu->programCounter + sizeof(uint64_t));
    cpu->programCounter = cpu->program;
    cpu->numberBytesFile -= (int)sizeof(uint64_t);

    return CPU_NO_ERROR;
}

char calculatedCMD(char codeOperation)
{
    return codeOperation & (0b11111);
}

stackData_t *getArg(short int codeOperation, struct CPU_t *cpu, CPU_ErrorCode *cpuError)
{
    assert(cpu != nullptr);
    assert(cpuError != nullptr);

    stackData_t *arg = nullptr;
    char numberRegister = 0;
    size_t numberDynMem = 0;

    if ((codeOperation & OPERATION_USE_MEM) != 0)
    {
        if ((codeOperation & OPERATION_USE_IMM) != 0)
        {
            numberDynMem = *(stackData_t*)(cpu->programCounter) - 1;
            if (numberDynMem < 0 || numberDynMem > SIZE_MEM - 1)
            {
                *cpuError = CPU_NO_NUMBER_DYN_MEM;
            }
            arg = &cpu->dynMem[numberDynMem];
            cpu->programCounter = (char*)(cpu->programCounter + sizeof(stackData_t));
        }
        else
        {
            numberRegister = *(char*)(cpu->programCounter) - 1;
            if (numberRegister < 0 || numberRegister > NUMBER_REG - 1)
            {
                *cpuError = CPU_NO_NUMBER_REGISTER;
            }
            arg = &cpu->dynMem[(long long int)cpu->registers[numberRegister] - 1];
            cpu->programCounter = (char*)(cpu->programCounter + sizeof(char));
        }
    }
    else
        {
        if ((codeOperation & OPERATION_USE_IMM) != 0)
        {
            arg = (stackData_t*)(cpu->programCounter);
            cpu->programCounter = (char*)(cpu->programCounter + sizeof(stackData_t));
        }
        else
        {
            numberRegister = *cpu->programCounter - 1;
            if (numberRegister < 0 || numberRegister > NUMBER_REG - 1)
            {
                *cpuError = CPU_NO_NUMBER_REGISTER;
            }
            arg = &cpu->registers[numberRegister];
            cpu->programCounter = (char*)(cpu->programCounter + sizeof(char));
        }
    }

    return arg;
}

CPU_ErrorCode executeCode(CPU_t *cpu)
{
    assert(cpu != nullptr);

    CPU_ErrorCode cpuError = CPU_NO_ERROR;
    stackData_t elem1 = 0;
    stackData_t elem2 = 0;
    stackData_t *arg = nullptr;
    short int codeOperation = 0;
    size_t addressJump = 0;
    size_t addressCall = 0;
    size_t addressRet = 0;

    int i = 0;
    while (cpu->programCounter < cpu->program + cpu->numberBytesFile)
    {
        #ifdef LOG_ENABLED
            printf("\naddress :    %p\n\n", cpu->programCounter);
        #endif // LOG_ENABLED

        codeOperation = *(short int*)(cpu->programCounter);
        cpu->programCounter += sizeof(short int);
        short int cmd = calculatedCMD(codeOperation);

        #ifdef LOG_ENABLED
            printf("\ncmd : %2d  i : %2d\n\n", cmd, i++);
        #endif // LOG_ENABLED

        switch (cmd)
        {
            #define DEF_CMD_(num, name, arg, code) \
                case CPU_##name:                   \
                code

            #include "../include/Commands.h"
            #undef COMMANDS_H_
            #undef DEF_CMD_
            default:
            {
                #ifdef LOG_ENABLED
                    printf("\n%s\n\n", cmd);
                #endif // LOG_ENABLED
                return CPU_NO_COMMAND;
            }
        }
    }

    free(cpu->program - 2 * sizeof(uint64_t));
    free(cpu->dynMem);
    return CPU_NO_ERROR;
}

void dumpCPU(CPU_t *cpu)
{
    assert(cpu != nullptr);

    int i = 0;

    for (i = 0; i < cpu->numberBytesFile; i++)
    {
        printf("%x ", cpu->programCounter[i]);
    }
}
