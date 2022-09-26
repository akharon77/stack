#include "stack.h"
#include <math.h>

const long int POISON = 0xDEADBEEF;

void StackCtor(Stack *stk, long int capacity)
{
    stk->data     = calloc(capacity, sizeof(Elem));

    StackSetSize    (stk, capacity);
    StackSetCapacity(stk, capacity);
    StackFillPoison (stk);
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
