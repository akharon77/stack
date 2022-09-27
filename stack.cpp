#include <math.h>
#include "stack.h"
#include "log.h"

const long int POISON = 0xDEADBEEF;

#define StackCtor(stk, capacity) \\
do {        \\
            \\
} while (0) \\

void StackCtor_(Stack *stk, long int capacity, long int line, const char* filename, const char* name)
{
    stk->data = (Elem*) calloc(capacity, sizeof(Elem));

    StackSetSize     (stk, capacity);
    StackSetCapacity (stk, capacity);
    StackCanaryUpdate(stk);
    StackFillPoison  (stk, 0, capacity - 1);
#ifdef _DEBUG
    StackSetLine    (stk, line);
    StackSetFilename(stk, filename);
    StackSetFuncname(stk, funcname);
    StackSetName    (stk, name)
#endif
}

void StackDtor(Stack *stk)
{
    free(stk->data);
}

void StackPush(Stack *stk, Elem el)
{
    StackResize (stk, stk->size + 1);
    StackSetData(stk, stk->size - 1, el);
}

void StackPop(Stack *stk)
{
    StackResize(stk, stk->size - 1);
}

Elem StackTop(Stack *stk)
{
    return StackGetData(stk, stk->size - 1);
}

void StackResize(Stack *stk, long int size)
{
    if (size >= stk->capacity)
    {
        long int incCoeffCapacity = stk->capacity * StackGetCoeff(stk);
        StackRealloc(stk, MAX(size, incCoeffCapacity));
    }
    else
    {
        long int decCoeffCapacity = stk->capacity / sqr(StackGetCoeff(stk));
        StackRealloc(stk, MAX(size, decCoeffCapacity));
    }

    StackSetSize(stk, size);
}

void StackRealloc(Stack *stk, long int capacity)
{
    if (capacity == stk->capacity)
        return;

    Elem *newData = realloc(stk->data, capacity);
    StackSetData    (stk, newData);
    StackSetCapacity(stk, capacity);
    StackFillPoison (stk, stk->size, set->capacity);
}

void StackSetSize(Stack *stk, long int size)
{
    stk->currSize = size;
}

void StackSetCapacity(Stack *stk, long int capacity)
{
    stk->capacity = capacity;
}

void StackSetData(Stack *stk, long int ind, Elem value)
{
    stk->data[ind] = value;
}

long int StackGetSize(Stack *stk)
{
    LOG(StackDump(stk));
    return stk->size;
}

long int StackGetCapacity(Stack *stk)
{
    return stk->capacity;
}

long int StackGetData(Stack *stk, long int ind)
{
    return stk->data[ind];
}

void StackFillPoison(Stack *stk, long int l, long int r)
{
    stk->canaryToken1 = stk->canaryToken2 = POISON;
    for (long int i = l; i < r; ++i)
        stk->data[i] = POISON;
}

void StackCanaryUpdate(Stack *stk)
{
    stk->canary1 = stk->canary2 = POISON;
}

#ifdef _DEBUG

void StackDump(Stack *stk, long int l, long int r)
{
    sprintf(logBuffer, "%s at %s(%d): \n"
                       "Stack[0x%x] %s 
}

#endif
