#ifndef COMMANDS_H_
#define COMMANDS_H_

#define LOG_ENABLED
#undef LOG_ENABLED

#include "Commands_help.h"

DEF_CMD_(1, CRT, 0,
            {
                IS_CPU_ERROR_(STACKCTOR_(&cpu->stack, 10));
                break;
            })

DEF_CMD_(2, IN, 0,
            {
                #ifdef LOG_ENABLED
                    printf("\nIN elem (1) : %lf\n", elem1);
                #endif // LOG_ENABLED
                
                if ( scanf("%lf", &elem1) != 1 )
                {
                    printf( "Error: scanf!\n" );
                }

                #ifdef LOG_ENABLED
                    printf("\nIN elem (2) : %lf\n\n", elem1);
                #endif // LOG_ENABLED

                PUSH_(elem1)
                break;
            })

DEF_CMD_(3, PUSH, 1,
            {
                arg = getArg(codeOperation, cpu, &cpuError);
                PUSH_(*arg)
                break;
            })

DEF_CMD_(4, ADD, 0,
            {
                POP_(&elem1)
                POP_(&elem2)
                PUSH_(elem1 + elem2)
                break;
            })

DEF_CMD_(5, SUB, 0,
            {
                POP_(&elem1)
                POP_(&elem2)
                PUSH_(elem2 - elem1)
                break;
            })

DEF_CMD_(6, MUL, 0,
            {
                POP_(&elem1)
                POP_(&elem2)
                PUSH_(elem2 * elem1)
                break;
            })

DEF_CMD_(7, DIV, 0,
            {
                POP_(&elem1)
                POP_(&elem2)
                PUSH_((stackData_t)(elem2 / elem1))
                break;
            })

DEF_CMD_(8, OUT, 0,
            {
                POP_(&elem1)
                printf("%g\n", elem1);
                break;
            })

DEF_CMD_(9, HLT, 0,
            {
                IS_CPU_ERROR_(StackDtor(&cpu->stack));
                cpu->programCounter = cpu->program + cpu->numberBytesFile;
                break;
            })

DEF_CMD_(10, POP, 1,
            {
                arg = getArg(codeOperation, cpu, &cpuError);
                POP_(arg)
                break;
            })

DEF_CMD_(11, JMP, 1, { JMP_()                                                              })

DEF_CMD_(12, JA,  1, { COND_JMP_(elem2 > elem1 + PRECISION)                                })

DEF_CMD_(13, JAE, 1, { COND_JMP_(elem2 >= elem1 + PRECISION)                               })

DEF_CMD_(14, JB,  1, { COND_JMP_(elem2 < elem1 - PRECISION)                                })

DEF_CMD_(15, JBE, 1, { COND_JMP_(elem2 <= elem1 - PRECISION)                               })

DEF_CMD_(16, JE,  1, { COND_JMP_(elem2 <= elem1 + PRECISION && elem2 >= elem1 - PRECISION) })

DEF_CMD_(17, JNE, 1, { COND_JMP_(elem2 > elem1 + PRECISION || elem2 < elem1 - PRECISION)   })

DEF_CMD_(18, CALL, 1,
        {
            addressCall = (size_t)(cpu->programCounter + sizeof(stackData_t));

            #ifdef LOG_ENABLED
                printf("\nCALL : %x\n\n", addressCall);
            #endif // LOG_ENABLED

            cpu->callArray[cpu->cntNumAddress++] = addressCall;
            JMP_()
        })

DEF_CMD_(19, RET, 0,
        {
            addressRet = cpu->callArray[--cpu->cntNumAddress];

            #ifdef LOG_ENABLED
                printf("\nRET : %x\n\n", addressRet);
            #endif // LOG_ENABLED

            cpu->programCounter = cpu->program + (addressRet - (size_t)cpu->program);
            break;
        })

DEF_CMD_(20, SQRT, 1,
        {
            arg = getArg(codeOperation, cpu, &cpuError);
            SQRT_(*arg)
        })

DEF_CMD_(21, DRAW, 0, {DRAW_()})

DEF_CMD_(22, MOVE, 0, {MOVE_()})

DEF_CMD_(23, P_CDOT, 0,
            {
                printf("*");
                break;
            })

DEF_CMD_(24, P_SPACE, 0,
            {
                printf(" ");
                break;
            })

DEF_CMD_(25, NEW_LINE, 0,
            {
                printf("\n");
                break;
            })


#endif // COMMANDS_H_
