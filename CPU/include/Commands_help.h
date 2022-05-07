#ifndef COMMANDS_HELP_H_
#define COMMANDS_HELP_H_

#define PUSH_(elem) \
    IS_CPU_ERROR_(StackPush(&cpu->stack, elem));

#define POP_(elem) \
    IS_CPU_ERROR_(StackPop(&cpu->stack, elem));

#define JMP_()                                          \
    addressJump = *(stackData_t*)(cpu->programCounter); \
    cpu->programCounter = cpu->program + addressJump;   \
    break;

#define COND_JMP_(condition)                        \
    POP_(&elem1)                                    \
    POP_(&elem2)                                    \
    if (condition)                                  \
    {                                               \
        JMP_()                                      \
    }                                               \
    else                                            \
    {                                               \
        cpu->programCounter += sizeof(stackData_t); \
        break;                                      \
    }

#define SQRT_(elem)    \
    elem = sqrt(elem); \
    break;

#define DRAW_()                            \
    int cnt = 0;                           \
    for (size_t i = 0; i < SIZE_MEM; i ++) \
    {                                      \
        if (cpu->dynMem[i] == 1.0)         \
            printf("*");                   \
        else if (cpu->dynMem[i] == -1)     \
            break;                         \
        else                               \
            printf(" ");                   \
        cnt++;                             \
        if (cnt == FIELD_WIDTH)            \
        {                                  \
            printf("\n");                  \
            cnt = 0;                       \
        }                                  \
    }                                      \
    break;

#define MOVE_()                       \
    char c = getchar();               \
    switch ((int)c)                   \
    {                                 \
        case (int)'a':                \
        {                             \
            PUSH_(-1)                 \
            break;                    \
        }                             \
        case (int)'d':                \
        {                             \
            PUSH_(1)                  \
            break;                    \
        }                             \
        case (int)'w':                \
        {                             \
            PUSH_(-FIELD_WIDTH)       \
            break;                    \
        }                             \
        case (int)'s':                \
        {                             \
            PUSH_(FIELD_WIDTH)        \
            break;                    \
        }                             \
        default:                      \
        {                             \
            PUSH_(0)                  \
            break;                    \
        }                             \
    }                                 \
    usleep(500000);                   \
    if ( system("clear") )            \
    {                                 \
        printf( "Error: system!\n" ); \
    }                                 \
    break;

#endif // COMMANDS_HELP_H_
