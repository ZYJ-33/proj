#include "soft_operators.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#define OPERATOR_COUNT 1
char* operators[] =
{
    "averge_pool",
};

extern void panic(char* msg);

u_int8_t get_opcode(char* func)
{
    for(u_int32_t i=0; i<OPERATOR_COUNT; i++)
    {
            if(strcmp(func, operators[i]) == 0)
                    return i;
    }
    printf("can't find soft_operator %s \n");
    exit(1);
}

