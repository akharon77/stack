#ifndef STACK_STRUCT_H
#define STACK_STRUCT_H

#include "stack_debug.h"
#include "stack_prot.h"

#define Elem int64_t

struct Stack
{
    int64_t size;
    int64_t capacity;
    Elem *data;

    ON_DEBUG(
    int64_t    line;
    const char* filename;
    const char* funcname;
    const char* name;
    )

    ON_HASH_PROT(
    uint64_t xCoeff;
    uint64_t hashData;
    uint64_t hashStk;
    )
};

#endif  // STACK_STRUCT_H
