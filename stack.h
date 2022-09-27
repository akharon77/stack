#ifndef STACKH
#define STACKH

#include <math.h>
#include "log.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))

#ifdef CANARY_PROT
#define ON_CANARY_PROT(...) __VA_ARGS__
#else
#define ON_CANARY_PROT(...)
#endif

#ifdef HASH_PROT
#define ON_HASH_PROT(...)   __VA_ARGS__
#else
#define ON_HASH_PROT(...)
#endif

#ifdef _DEBUG
#define ON_DEBUG(...)       __VA_ARGS__
#else
#define ON_DEBUG(...)
#endif

const long int POISON = 0xDEADBEEF;
const long int CANARY = 0xBADC0FFEE;

struct Stack
{
    long int size;
    long int capacity;
    Elem *data;

    ON_DEBUG(
        long int    line;
        const char* filename;
        const char* funcname;
        const char* name;
    )

    ON_HASH_PROT(
        unsigned long long hashData;
        unsigned long long hashStk;
    )
};

void StackCtor_(
                Stack *stk, 
                long int capacity 
                ON_DEBUG(, long int line)
                ON_DEBUG(, const char* filename)
                ON_DEBUG(, const char* funcname)
                ON_DEBUG(, const char* name)
               );


#define StackCtor(stk, capacity)               \\
StackCtor_(                                    \\
           stk,                                \\
           capacity                            \\
           ON_DEBUG(, __LINE__)                \\
           ON_DEBUG(, __FILE__)                \\
           ON_DEBUG(, __FUNCTION__)            \\
           ON_DEBUG(, #stk + (#stk[0] == '&')) \\
          )

void StackCtor_(
                Stack *stk, 
                long int capacity 
                ON_DEBUG(, long int line)
                ON_DEBUG(, const char* filename)
                ON_DEBUG(, const char* funcname)
                ON_DEBUG(, const char* name)
               )
{
    stk->data = (Elem*) calloc(capacity ON_CANARY_PROT(+2), sizeof(Elem));

    StackSetData    (stk, data ON_CANARY_PROT(+1));
    StackSetSize    (stk, capacity);
    StackSetCapacity(stk, capacity);
    StackFillPoison (stk, 0, capacity);

    ON_CANARY_PROT(StackCanaryUpdate(stk);)

    ON_DEBUG(StackSetLine    (stk, line);)
    ON_DEBUG(StackSetFilename(stk, filename);)
    ON_DEBUG(StackSetFuncname(stk, funcname);)
    ON_DEBUG(StackSetName    (stk, name))
}

void StackDtor(Stack *stk)
{
    free(stk->data ON_CANARY_PROT(-1));
}

// ----------------------------

void StackPush(Stack *stk, Elem el)
{
    StackResize (stk, stk->size + 1);
    StackSetData(stk, stk->size - 1, el);
}

Elem StackTop(Stack *stk)
{
    return StackGetData(stk, stk->size - 1);
}

void StackPop(Stack *stk)
{
    Elem res = StackTop(stk);
    StackResize(stk, stk->size - 1);
    return res;
}

// --------------RE-METHODS--------------
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

    Elem *newData = realloc(stk->data, capacity ON_CANARY_PROT(+2));
    // if null

    StackSetData    (stk, newData ON_CANARY_PROT(+1));
    StackSetCapacity(stk, capacity);
    StackFillPoison (stk, stk->size, set->capacity);

    ON_CANARY_PROT(StackCanaryUpdate(stk);)
}

// --------------SETTERS--------------

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
    stk->data[ind ON_CANARY_PROT(+1)] = value;
}

void StackFillPoison(Stack *stk, long int l, long int r)
{
    for (long int i = l; i < r; ++i)
        StackSetData(stk, i, POISON);
}

ON_CANARY_PROT(
    void StackCanaryUpdate(Stack *stk)
    {
        stk->data[-1]       = CANARY;
        stk->data[capacity] = CANARY;
    }
)

// --------------GETTERS--------------

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
    return stk->data[ind ON_CANARY_PROT(+1)];
}

ON_DEBUG(
    long int StackGetLine(Stack *stk)
    {
        return stk->line;
    }

    const char* StackGetFilename(Stack *stk)
    {
        return stk->filename;
    }

    const char* StackGetFuncname(Stack *stk)
    {
        return stk->funcname;
    }

    const char* StackGetName(Stack *stk)
    {
        return stk->name;
    }
)

// --------------error??--------------

ON_DEBUG(
    int getLogBuffer()
    {
        
    }
    void StackDump(Stack *stk, long int l, long int r)
    {
        int logBuffer = getLogBuffer();
        sprintf(logBuffer, "Stack[%p] \"%s\" at %s in %s(%d): \n"
                           "{\n\tsize = %d, \n",
                           "\tcapacity = %d, \n",
                           "\tdata[%p]\n",
                           stk, stk->name, stk->funcname, stk->filename, stk->line,
                           stk->size,
                           stk->capacity,
                           stk->data);
    }

)

#endif
