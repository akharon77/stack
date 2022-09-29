#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "stack.h"

const long int POISON = 0xDEADBEEF;
const long int CANARY = 0xBADC0FFEE;

void StackCtor_(
                Stack *stk, 
                long int capacity 
                ON_DEBUG(, long int line)
                ON_DEBUG(, const char* filename)
                ON_DEBUG(, const char* funcname)
                ON_DEBUG(, const char* name)
               )
{
    capacity += 0 ON_CANARY_PROT(+2);
    Elem* data = (Elem*) calloc(capacity, sizeof(Elem));

    stk->data = data ON_CANARY_PROT(+1);
    stk->size = 0;
    stk->capacity = capacity;
    StackFillPoison (stk, 0, stk->capacity);

    ON_CANARY_PROT(StackCanaryUpdate(stk);)

    ON_DEBUG(StackSetLine     (stk, line);)
    ON_DEBUG(StackSetFilename (stk, filename);)
    ON_DEBUG(StackSetFuncname (stk, funcname);)
    ON_DEBUG(StackSetName     (stk, name);)

    LOG(STACK_CTOR, stk);
}

void StackDtor(Stack *stk)
{
    free(stk->data ON_CANARY_PROT(-1));
    LOG(STACK_DTOR, stk);
    ON_DEBUG(closeLogBuffer();)
}

// ----------------------------

void StackPush(Stack *stk, Elem el)
{
    StackResize (stk, stk->size + 1);
    StackSetData(stk, stk->size - 1, el);
    LOG(STACK_PUSH, stk);
}

Elem StackTop(Stack *stk)
{
    LOG(STACK_TOP, stk);
    return StackGetData(stk, stk->size - 1);
}

Elem StackPop(Stack *stk)
{
    Elem res = StackTop(stk);
    StackResize(stk, stk->size - 1);
    LOG(STACK_POP, stk);
    return res;
}

// --------------RE-METHODS--------------
void StackResize(Stack *stk, long int size)
{
    if (size > stk->capacity)
    {
        long int incCoeffCapacity = stk->capacity * StackGetCoeff(stk);
        StackRealloc(stk, MAX(size, incCoeffCapacity));
    }
    else if (size < stk->size)
    {
        long int coeff = StackGetCoeff(stk);
        long int decCoeffCapacity = stk->capacity / (coeff * coeff);
        StackRealloc(stk, MAX(size, decCoeffCapacity));
    }

    StackSetSize(stk, size);
    LOG(STACK_RESIZE, stk);
}

void StackRealloc(Stack *stk, long int capacity)
{
    if (capacity == stk->capacity)
        return;

    Elem *newData = (Elem*) realloc(stk->data, capacity ON_CANARY_PROT(+2));
    // if null

    StackSetDataMem (stk, newData ON_CANARY_PROT(+1));
    StackSetCapacity(stk, capacity);
    StackFillPoison (stk, stk->size, stk->capacity);

    ON_CANARY_PROT(StackCanaryUpdate(stk);)
    LOG(STACK_REALLOC, stk);
}

// --------------SETTERS--------------

void StackSetSize(Stack *stk, long int size)
{
    stk->size = size;
    LOG(STACK_SET_SIZE, stk);
}

void StackSetCapacity(Stack *stk, long int capacity)
{
    stk->capacity = capacity;
    LOG(STACK_SET_CAPACITY, stk);
}

void StackSetDataMem(Stack *stk, Elem *data)
{
    stk->data = data;
    LOG(STACK_SET_DATA_MEM, stk);
}
void StackSetData(Stack *stk, long int ind, Elem value)
{
    stk->data[ind] = value;
    LOG(STACK_SET_DATA, stk);
}

void StackFillPoison(Stack *stk, long int l, long int r)
{
    for (long int i = l; i < r; ++i)
        StackSetData(stk, i, POISON);
    LOG(STACK_FILL_POISON, stk);
}

ON_CANARY_PROT(
    void StackCanaryUpdate(Stack *stk)
    {
        stk->data[-1]                = CANARY;
        stk->data[stk->capacity - 1] = CANARY;
        LOG(STACK_CANARY_UPDATE, stk);
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

Elem *StackGetDataMem(Stack *stk)
{
    return stk->data;
}

long int StackGetCoeff(Stack *stk)
{
    return 2;
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
        LOG(STACK_GET_FUNCNAME, stk);
        return stk->funcname;
    }

    const char* StackGetName(Stack *stk)
    {
        LOG(STACK_GET_NAME, stk);
        return stk->name;
    }

    void StackSetLine(Stack *stk, long int line)
    {
        LOG(STACK_SET_LINE, stk);
        stk->line = line;
    }

    void StackSetFilename(Stack *stk, const char *filename)
    {
        LOG(STACK_SET_FILENAME, stk);
        stk->filename = filename;
    }

    void StackSetFuncname(Stack *stk, const char *funcname)
    {
        LOG(STACK_SET_FUNCNAME, stk);
        stk->funcname = funcname;
    }

    void StackSetName(Stack *stk, const char *name)
    {
        LOG(STACK_SET_NAME, stk);
        stk->name = name;
    }
)

// --------------error??--------------

ON_DEBUG(
    void StackDump(Stack *stk)
    {
        int fdLogBuffer = getfdLogBuffer();
        dprintf(fdLogBuffer, "Stack[%p] \"%s\" at %s in %s(%ld): \n"
                           "{\n\tsize = %ld, \n"
                           "\tcapacity = %ld, \n"
                           "\tdata[%p]\n{\n",
                           stk, StackGetName(stk), StackGetFuncname(stk), StackGetFilename(stk), StackGetLine(stk),
                           StackGetSize(stk),
                           StackGetCapacity(stk),
                           StackGetDataMem(stk));
        for (long int i = 0; i < StackGetCapacity(stk); ++i)
        {
            dprintf(fdLogBuffer, "\t[%ld] = %ld\n",
                               i, stk->data[i ON_CANARY_PROT(-1)]);
        }
        dprintf(fdLogBuffer, "}\n");
    }

    static int fdLogBuffer = -1;

    int getfdLogBuffer()
    {
        if (fdLogBuffer == -1)
        {
            fdLogBuffer = creat(LOG_FILENAME, S_IRWXU);
        }
        return fdLogBuffer;
    }

    void closeLogBuffer()
    {
        close(fdLogBuffer);
        fdLogBuffer = -1;
    }

    void logToBuffer(const char* event, Stack *stk)
    {
        ON_DEBUG(
            fdLogBuffer = getfdLogBuffer();
            dprintf(fdLogBuffer, "[%s]\n", event);
            StackDump(stk);
        )
    }
)
