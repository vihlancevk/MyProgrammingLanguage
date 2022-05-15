#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#define DEBUG
// #undef  DEBUG

#define IS_STACK_ERROR_(stack, stackError)    \
    if (stackError != STACK_NO_ERROR)         \
    {                                         \
        /* ASSERT_OK(stack); */               \
        return stackError;                    \
    }

typedef int stackData_t;

enum StackStatus
{
    STACK_NOT_CONSTRUCTED,
    STACK_CONSTRUCTED,
    STACK_DESTROYED
};

struct stack_t
{
    #ifdef DEBUG
        uint64_t leftStackCanary;
    #endif // DEBUG

    size_t capacity;
    stackData_t *data;
    size_t size;
    StackStatus status;

    #ifdef DEBUG
        const char *nameStack;
        const char *filePath;
        size_t line;
        uint64_t rightStackCanary;
        uint64_t hashStack;
        uint64_t hashData;
    #endif // DEBUG
};

enum StackErrorCode
{
    STACK_NO_ERROR,
    STACK_NOT_EXIST,
    STACK_ALREADY_CONSTRUCTED,
    STACK_DATA_NO_CREATE,
    STACK_DATA_DESTROY,
    STACK_POP_FROM_EMPTY,
    STACK_DATA_CALLOC_ERROR,
    STACK_DATA_REALLOC_ERROR,
    STACK_CAPACITY_LESS_SIZE,
    STACK_NOT_SUPPORT_OPERATION,

    #ifdef DEBUG
        STACK_LEFT_CANARY_DIED,
        STACK_RIGHT_CANARY_DIED,
        STACK_NO_NAME,
        STACK_HASH_DAMAGED,
        DATA_HASH_DAMAGED,
        DATA_LEFT_CANARY_DIED,
        DATA_RIGHT_CANARY_DIED
    #endif // DEBUG
};

//{-------------------------------------------------------------------

void ClearLogFile();

#ifdef DEBUG
    #define CLEAR_LOG_FILE() \
    ClearLogFile()
#else
    #define CLEAR_LOG_FILE() \

#endif // DEBUG

//}-------------------------------------------------------------------

//{-------------------------------------------------------------------

StackErrorCode GetStackError(stack_t *stack);

#ifdef DEBUG
    void Dump(stack_t *stack, const char *nameFunction, size_t line, const char *filePath);
#endif // DEBUG

#ifdef DEBUG
    #define ASSERT_OK(stack)                                        \
        do{                                                         \
        /* Dump(stack, __PRETTY_FUNCTION__, __LINE__, __FILE__); */ \
        /* assert(GetStackError(stack) == STACK_NO_ERROR); */       \
        }while(false);
#else
    #define ASSERT_OK(stack)                                      \

#endif //DEBUG

//}-------------------------------------------------------------------

//{-------------------------------------------------------------------

StackErrorCode StackCtor(stack_t *stack, size_t capacity);

#ifdef DEBUG
    StackErrorCode StackCtorForDebug(stack_t *stack, size_t capacity, const char *nameStack, size_t line, const char* filePath);
#endif // DEBUG

#ifdef DEBUG
    #define STACKCTOR_(stack, capacity)                                      \
        /* StackCtorForDebug(stack, capacity, #stack, __LINE__, __FILE__) */ \
        StackCtor(stack, capacity)
#else
    #define STACKCTOR_(stack, capacity)                                      \
        StackCtor(stack, capacity)
#endif // DEBUG

//}-------------------------------------------------------------------

StackErrorCode StackDtor(stack_t *stack);

StackErrorCode StackPush(stack_t *stack, stackData_t element);

StackErrorCode StackPop(stack_t *stack, stackData_t *top);

size_t GetStackCapacity(stack_t *stack);

size_t GetStackSize(stack_t *stack);

#endif
