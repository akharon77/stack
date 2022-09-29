#ifndef STACKH
#define STACKH

#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))

#ifdef CANARY_PROT
#define ON_CANARY_PROT(...) __VA_ARGS__
#else
#define ON_CANARY_PROT(...)
#endif

#ifdef HASH_PROT
#define ON_HASH_PROT(...) __VA_ARGS__
#else
#define ON_HASH_PROT(...)
#endif

#ifdef _DEBUG
#define LOG_FILENAME "stack.log"
#define STACK_OK(stk) StackDump(stk, __PRETTY_FUNCTION__, __FILE__, __LINE__)
#define ON_DEBUG(...) __VA_ARGS__
#else
#define ON_DEBUG(...)
#endif

const long int POISON = 0xDEADBEEF;
const long int CANARY = 0xBADC0FFEE;

enum STACK_ERRORS {
    STACK_SIZE_G_CAPACITY_ERROR  = 1 << 0,
    STACK_CANARY_OVERWRITE_ERROR = 1 << 1,
    STACK_HASH_DISPARITY_ERROR   = 1 << 2,
    STACK_DATA_POSION_ERROR      = 1 << 3
};

#define Elem long int

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

#ifdef _DEBUG
#define StackCtor(stk, capacity)   \
StackCtor_(                        \
           stk,                    \
           capacity,               \
           __LINE__,               \
           __FILE__,               \
           __FUNCTION__,           \
           #stk + (#stk[0] == '&') \
          )
#else
#define StackCtor(stk, capacity)   \
StackCtor_(                        \
           stk,                    \
           capacity,               \
          )
#endif

void StackCtor_(
                Stack *stk,
                long int capacity 
                ON_DEBUG(, long int line)
                ON_DEBUG(, const char* filename)
                ON_DEBUG(, const char* funcname)
                ON_DEBUG(, const char* name)
               );

void        StackDtor         (Stack *stk);
void        StackPush         (Stack *stk, Elem el);
Elem        StackTop          (Stack *stk);
Elem        StackPop          (Stack *stk);
void        StackResize       (Stack *stk, long int size);
void        StackRealloc      (Stack *stk, long int capacity);
long int    StackError        (Stack *stk);
void        StackSetSize      (Stack *stk, long int size);
void        StackSetCapacity  (Stack *stk, long int capacity);
void        StackSetDataMem   (Stack *stk, Elem *data);
void        StackSetData      (Stack *stk, long int ind, Elem value);
void        StackFillPoison   (Stack *stk, long int l, long int r);
long int    StackGetSize      (Stack *stk);
long int    StackGetCapacity  (Stack *stk);
long int    StackGetData      (Stack *stk, long int ind);
Elem*       StackGetDataMem   (Stack *stk);
long int    StackGetCoeff     (Stack *stk);
ON_CANARY_PROT(
void        StackCanaryUpdate (Stack *stk);
)
ON_DEBUG(
void        StackDump        (Stack *stk);
const char* StackGetStatus   (Stack *stk);
long int    StackGetLine     (Stack *stk);
const char* StackGetName     (Stack *stk);
const char* StackGetFuncname (Stack *stk);
const char* StackGetFilename (Stack *stk);
void        StackSetLine     (Stack *stk, long int line);
void        StackSetName     (Stack *stk, const char *name);
void        StackSetFuncname (Stack *stk, const char *funcname);
void        StackSetFilename (Stack *stk, const char *filename);
int         getfdLogBuffer   ();
void        closeLogBuffer   ();
void        logToBuffer      (const char* event, Stack *stk);
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
    Elem* data = (Elem*) calloc(capacity ON_CANARY_PROT(+2), sizeof(Elem));

    stk->data     = data ON_CANARY_PROT(+1);
    stk->size     = 0;
    stk->capacity = capacity;
    stk->flags    = 0;
    StackFillPoison (stk, 0, stk->capacity);

    ON_CANARY_PROT(StackCanaryUpdate(stk);)

    ON_DEBUG(StackSetLine     (stk, line);)
    ON_DEBUG(StackSetFilename (stk, filename);)
    ON_DEBUG(StackSetFuncname (stk, funcname);)
    ON_DEBUG(StackSetName     (stk, name);)

    STACK_OK(stk);
}

void StackDtor(Stack *stk)
{
    free(stk->data ON_CANARY_PROT(-1));
    LOG(STACK_DTOR, stk);
    ON_DEBUG(closeLogBuffer();)
    STACK_OK(stk);
}

// ----------------------------

void StackPush(Stack *stk, Elem el)
{
    StackResize (stk, stk->size + 1);
    StackSetData(stk, stk->size - 1, el);
    STACK_OK(stk);
}

Elem StackTop(Stack *stk)
{
    STACK_OK(stk);
    return StackGetData(stk, stk->size - 1);
}

Elem StackPop(Stack *stk)
{
    Elem res = StackTop(stk);
    StackSetData(stk, stk->size - 1, POISON);
    StackResize(stk, stk->size - 1);
    STACK_OK(stk);
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
    STACK_OK(stk);
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

    STACK_OK(stk);
}

// --------------SETTERS--------------

void StackSetSize(Stack *stk, long int size)
{
    stk->size = size;
    STACK_OK(stk);
}

void StackSetCapacity(Stack *stk, long int capacity)
{
    stk->capacity = capacity;
    STACK_OK(stk);
}

void StackSetDataMem(Stack *stk, Elem *data)
{
    stk->data = data;
    STACK_OK(stk);
}
void StackSetData(Stack *stk, long int ind, Elem value)
{
    stk->data[ind] = value;
    STACK_OK(stk);
}

void StackFillPoison(Stack *stk, long int l, long int r)
{
    for (long int i = l; i < r; ++i)
        StackSetData(stk, i, POISON);
    STACK_OK(stk);
}

ON_CANARY_PROT(
    void StackCanaryUpdate(Stack *stk)
    {
        stk->data[-1]            = CANARY;
        stk->data[stk->capacity] = CANARY;
        STACK_OK(stk);
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
    return stk->data[ind];
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
        return stk->funcname;
    }

    const char* StackGetName(Stack *stk)
    {
        return stk->name;
    }

    void StackSetLine(Stack *stk, long int line)
    {
        stk->line = line;
        STACK_OK(stk);
    }

    void StackSetFilename(Stack *stk, const char *filename)
    {
        stk->filename = filename;
        STACK_OK(stk);
    }

    void StackSetFuncname(Stack *stk, const char *funcname)
    {
        stk->funcname = funcname;
        STACK_OK(stk);
    }

    void StackSetName(Stack *stk, const char *name)
    {
        stk->name = name;
        STACK_OK(stk);
    }
)

// --------------error??--------------

ON_DEBUG(
void StackDump(Stack *stk, const char* func, const char* file, long int line)
{
    int fdLogBuffer = getfdLogBuffer();

    dprintf(fdLogBuffer,"StackDump: from %s in %s(%d)\n",
                        "Stack[%p]\n"
                        "Status:\n"
                        "\"%s\" at %s in %s(%ld): \n"
                        "{\n\tsize = %ld, \n"
                        "\tcapacity = %ld, \n"
                        "\tdata[%p]\n{\n",
                        func, file, line);
                        stk,
                        StackGetStatus(stk),
                        StackGetName(stk), StackGetFuncname(stk), StackGetFilename(stk), StackGetLine(stk),
                        StackGetSize(stk),
                        StackGetCapacity(stk),
                        StackGetDataMem(stk));
ON_CANARY_PROT(
        dprintf(fdLogBuffer, "\t(c)]t[-1] %ld",
                             StackGetDataMem(stk)[-1]);
)

    for (long int i = 0; i < StackGetSize(stk); ++i)
        dprintf(fdLogBuffer, "\t(*)\t[%ld] = %ld\n",
                             i, StackGetData(stk, i));
    for (long int i = StackGetSize(stk); i < StackGetCapacity(stk); ++i)
        dprintf(fdLogBuffer, "\t\t[%ld] = %ld\n",
                             i, StackGetData(stk, i));

ON_CANARY_PROT(
        dprintf(fdLogBuffer, "\t(c)\t[%ld] %ld",
                             StackGetCapacity(stk) + 1,
                             StackGetData(stk, StackGetCapacity()));
)
    dprintf(fdLogBuffer, "}\n");
}

int fdLogBuffer = -1;

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

const char* StackGetStatus (Stack *stk)
{
    
}

long int StackError(Stack *stk)
{
    long int flags = 0;

    flags |= (StackGetSize(stk) < StackGetCapacity(stk))          ? STACK_SIZE_G_CAPACITY_ERROR  : 0;
ON_CANARY_PROT(
    flags |= (StackGetData(stk, -1) != CANARY ||
              StackGetData(stk, StackGetCapacity(stk)) != CANARY) ? STACK_CANARY_OVERWRITE_ERROR : 0;
)
ON_HASH_PROT(
    flags |= (StackEvaluateHash(stk) != StackGetHash(stk))        ? STACK_HASH_DISPARITY_ERROR   : 0;
)
    for (long int i = StackGetSize(stk); i < StackGetCapacity(stk); ++i)
        flags |= (StackGetData(stk, i) != POISON                  ? STACK_DATA_POSION_ERROR      : 0;
}
    return flags;
)

#endif
