#ifndef STACKH
#define STACKH

#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

#define ASSERT(expr)                                        \
do {                                                        \
    if (!(expr))                                            \
        fprintf(stderr,                                     \
                "Assertion failed. Expression: " #expr "\n" \
                "File: %s, line: %d\n"                      \
                "Function: %s\n",                           \
                __FILE__, __LINE__,                         \
                __PRETTY_FUNCTION__);                       \
} while(0)

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
#define ON_DEBUG(...) __VA_ARGS__
#else
#define ON_DEBUG(...)
#endif

#define STACK_OK(stk)                                        \
do                                                           \
{                                                            \
    if (StackError(stk) != 0) {                              \
        printf(StackGetStatus(stk));                         \
        abort();                                             \
    }                                                        \
    StackDump(stk, __PRETTY_FUNCTION__, __FILE__, __LINE__); \
}                                                            \
while(0)

const long int POISON = 0xDEADBEEF;
const long int CANARY = 0xBADC0FFEE;

enum STACK_ERRORS {
    STACK_SIZE_G_CAPACITY_ERROR     = 1 << 0,
    STACK_SIZE_NEG_ERROR            = 1 << 1,
    STACK_CAPACITY_NEG_ERROR        = 1 << 2,
ON_CANARY_PROT(
    STACK_CANARY_OVERWRITE_ERROR    = 1 << 3,
)
ON_HASH_PROT(
    STACK_HASH_DATA_DISPARITY_ERROR = 1 << 4,
    STACK_HASH_STK_DISPARITY_ERROR  = 1 << 5,
)
    STACK_DATA_POSION_ERROR         = 1 << 6
};

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
    uint64_t hashData;
    uint64_t hashStk;
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
           capacity                \
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

void        StackDtor             (Stack *stk);
void        StackPush             (Stack *stk, Elem el);
Elem        StackTop              (Stack *stk);
Elem        StackPop              (Stack *stk);
void        StackResize           (Stack *stk, long int size);
void        StackRealloc          (Stack *stk, long int capacity);
uint32_t    StackError            (Stack *stk);
void        StackSetSize          (Stack *stk, long int size);
void        StackSetCapacity      (Stack *stk, long int capacity);
void        StackSetDataMem       (Stack *stk, Elem *data);
void        StackSetData          (Stack *stk, long int ind, Elem value);
void        StackFillPoison       (Stack *stk, long int l, long int r);
long int    StackGetSize          (Stack *stk);
long int    StackGetCapacity      (Stack *stk);
long int    StackGetData          (Stack *stk, long int ind);
Elem*       StackGetDataMem       (Stack *stk);
long int    StackGetCoeff         (Stack *stk);
bool        isBadPtr              (void *ptr);
ON_CANARY_PROT(
void        StackCanaryUpdate     (Stack *stk);
)
ON_DEBUG(
void        StackDump             (Stack *stk);
const char* StackGetStatus        (Stack *stk);
long int    StackGetLine          (Stack *stk);
const char* StackGetName          (Stack *stk);
const char* StackGetFuncname      (Stack *stk);
const char* StackGetFilename      (Stack *stk);
void        StackSetLine          (Stack *stk, long int line);
void        StackSetName          (Stack *stk, const char *name);
void        StackSetFuncname      (Stack *stk, const char *funcname);
void        StackSetFilename      (Stack *stk, const char *filename);
int         getfdLogBuffer        ();
void        closeLogBuffer        ();
void        logToBuffer           (const char* event, Stack *stk);
)
ON_HASH_PROT(
uint64_t    StackEvaluateHashData (Stack *stk);
uint64_t    StackEvaluateHashStk  (Stack *stk);
void        StackRehash           (Stack *stk);
uint64_t    StackGetHashStk       (Stack *stk);
uint64_t    StackGetHashData      (Stack *stk);
void        StackSetHashStk       (Stack *stk, uint64_t hashStk);
void        StackSetHashData      (Stack *stk, uint64_t hashData);
)
void        StackDump             (Stack *stk, const char* func, const char* file, long int line);
const char* StackGetStatus        (Stack *stk);

void StackCtor_(
                Stack *stk, 
                long int capacity 
                ON_DEBUG(, long int line)
                ON_DEBUG(, const char* filename)
                ON_DEBUG(, const char* funcname)
                ON_DEBUG(, const char* name)
               )
{
    ASSERT(!isBadPtr(stk));
    Elem* data = (Elem*) calloc(capacity ON_CANARY_PROT(+2), sizeof(Elem));

    ASSERT(!isBadPtr(stk));
    stk->data     = data ON_CANARY_PROT(+1);
    stk->size     = 0;
    stk->capacity = capacity;
    StackFillPoison (stk, 0, stk->capacity);

    ON_CANARY_PROT(StackCanaryUpdate(stk);)

    ON_DEBUG(StackSetLine     (stk, line);)
    ON_DEBUG(StackSetFilename (stk, filename);)
    ON_DEBUG(StackSetFuncname (stk, funcname);)
    ON_DEBUG(StackSetName     (stk, name);)

    ON_HASH_PROT(StackRehash(stk);)
}

void StackDtor(Stack *stk)
{
    STACK_OK(stk);
    ASSERT(!isBadPtr(stk));
    free(stk->data ON_CANARY_PROT(-1));
    ON_DEBUG(closeLogBuffer();)
}

// ----------------------------

void StackPush(Stack *stk, Elem el)
{
    STACK_OK(stk);
    ASSERT(!isBadPtr(stk));
    StackResize (stk, stk->size + 1);
    StackSetData(stk, stk->size - 1, el);
    ON_HASH_PROT(StackRehash(stk);)
}

Elem StackTop(Stack *stk)
{
    STACK_OK(stk);
    ASSERT(!isBadPtr(stk));
    ASSERT(StackGetSize(stk) > 0);
    return StackGetData(stk, stk->size - 1);
}

Elem StackPop(Stack *stk)
{
    STACK_OK(stk);
    ASSERT(!isBadPtr(stk));
    if (StackGetSize(stk) < 0)
    {
        printf("Stack is empty\n");
    }
    ASSERT(StackGetSize(stk) > 0);
    Elem res = StackTop(stk);
    StackSetData(stk, stk->size - 1, POISON);
    StackResize(stk, stk->size - 1);
    ON_HASH_PROT(StackRehash(stk);)
    return res;
}

// --------------RE-METHODS--------------
void StackResize(Stack *stk, long int size)
{
    STACK_OK(stk);
    ASSERT(!isBadPtr(stk));

    if (size > stk->capacity)
    {
        long int incCoeffCapacity = stk->capacity * StackGetCoeff(stk);
        StackRealloc(stk, MAX(size, incCoeffCapacity));
    }
    else if (size < stk->size)
    {
        long int coeff = StackGetCoeff(stk);
        long int decCoeffCapacity = stk->capacity / (coeff * coeff);
        if (size < decCoeffCapacity)
            StackRealloc(stk, decCoeffCapacity);
    }

    StackSetSize(stk, size);
    ON_HASH_PROT(StackRehash(stk);)
}

void StackRealloc(Stack *stk, long int capacity)
{
    STACK_OK(stk);
    ASSERT(!isBadPtr(stk));

    if (capacity == stk->capacity)
        return;

    Elem *newData = (Elem*) realloc(stk->data ON_CANARY_PROT(-1), sizeof(Elem) * (capacity ON_CANARY_PROT(+2)));
    ASSERT(!isBadPtr(newData));

    StackSetDataMem (stk, newData ON_CANARY_PROT(+1));
    StackSetCapacity(stk, capacity);
    StackFillPoison (stk, stk->size, stk->capacity);
    ON_CANARY_PROT(StackCanaryUpdate(stk);)
    ON_HASH_PROT(StackRehash(stk);)
}

// --------------SETTERS--------------

void StackSetSize(Stack *stk, long int size)
{
    ASSERT(!isBadPtr(stk));
    stk->size = size;
    ON_HASH_PROT(StackRehash(stk);)
}

void StackSetCapacity(Stack *stk, long int capacity)
{
    ASSERT(!isBadPtr(stk));
    stk->capacity = capacity;
    ON_HASH_PROT(StackRehash(stk);)
}

void StackSetDataMem(Stack *stk, Elem *data)
{
    ASSERT(!isBadPtr(stk));
    stk->data = data;
    ON_HASH_PROT(StackRehash(stk);)
}
void StackSetData(Stack *stk, long int ind, Elem value)
{
    ASSERT(!isBadPtr(stk));
    stk->data[ind] = value;
    ON_HASH_PROT(StackRehash(stk);)
}

void StackFillPoison(Stack *stk, long int l, long int r)
{
    ASSERT(!isBadPtr(stk));
    for (long int i = l; i < r; ++i)
        StackSetData(stk, i, POISON);
    ON_HASH_PROT(StackRehash(stk);)
}

ON_CANARY_PROT(
    void StackCanaryUpdate(Stack *stk)
    {
        ASSERT(!isBadPtr(stk));
        stk->data[-1]            = CANARY;
        stk->data[stk->capacity] = CANARY;
        ON_HASH_PROT(StackRehash(stk);)
    }
)

// --------------GETTERS--------------

long int StackGetSize(Stack *stk)
{
    ASSERT(!isBadPtr(stk));
    return stk->size;
}

long int StackGetCapacity(Stack *stk)
{
    ASSERT(!isBadPtr(stk));
    return stk->capacity;
}

long int StackGetData(Stack *stk, long int ind)
{
    ASSERT(!isBadPtr(stk));
    return stk->data[ind];
}

Elem *StackGetDataMem(Stack *stk)
{
    ASSERT(!isBadPtr(stk));
    return stk->data;
}

long int StackGetCoeff(Stack *stk)
{
    ASSERT(!isBadPtr(stk));
    return 2;
}

ON_DEBUG(
    long int StackGetLine(Stack *stk)
    {
        ASSERT(!isBadPtr(stk));
        return stk->line;
    }

    const char* StackGetFilename(Stack *stk)
    {
        ASSERT(!isBadPtr(stk));
        return stk->filename;
    }

    const char* StackGetFuncname(Stack *stk)
    {
        ASSERT(!isBadPtr(stk));
        return stk->funcname;
    }

    const char* StackGetName(Stack *stk)
    {
        ASSERT(!isBadPtr(stk));
        return stk->name;
    }

    void StackSetLine(Stack *stk, long int line)
    {
        ASSERT(!isBadPtr(stk));
        stk->line = line;
        ON_HASH_PROT(StackRehash(stk);)
    }

    void StackSetFilename(Stack *stk, const char *filename)
    {
        ASSERT(!isBadPtr(stk));
        stk->filename = filename;
        ON_HASH_PROT(StackRehash(stk);)
    }

    void StackSetFuncname(Stack *stk, const char *funcname)
    {
        ASSERT(!isBadPtr(stk));
        stk->funcname = funcname;
        ON_HASH_PROT(StackRehash(stk);)
    }

    void StackSetName(Stack *stk, const char *name)
    {
        ASSERT(!isBadPtr(stk));
        stk->name = name;
        ON_HASH_PROT(StackRehash(stk);)
    }
)

// --------------error??--------------

const char* StackGetStatus(Stack *stk)
{
    ASSERT(!isBadPtr(stk));
    long int flags = StackError(stk);
    if (flags == 0)
    {
        return "(ok)\n";
    }
    if (flags | STACK_SIZE_NEG_ERROR)
        return "Stack size is negative\n";
    if (flags | STACK_CAPACITY_NEG_ERROR)
        return "Stack capacity is negative\n";
    if (flags | STACK_SIZE_G_CAPACITY_ERROR)
        return "Stack size is greater than capacity\n";
ON_CANARY_PROT(
    if (flags | STACK_CANARY_OVERWRITE_ERROR)
        return "Stack integrity has been violated, maybe array out of bounds occurred\n";
)
ON_HASH_PROT(
    if (flags | STACK_HASH_DATA_DISPARITY_ERROR)
        return "Stack integrity has been violated, an arbitrary element of the stack was accessed\n";
    if (flags | STACK_HASH_STK_DISPARITY_ERROR)
        return "Stack integrity has been violated, stack structure was corrputed\n";
)
    if (flags | STACK_DATA_POSION_ERROR)
        return "Stack integrity has been violated, unused stack memory accessed\n";
}

uint32_t StackError(Stack *stk)
{
    ASSERT(!isBadPtr(stk));
    long int flags = 0;

    flags |= ((StackGetSize(stk)     < 0)                          ? STACK_SIZE_NEG_ERROR         : 0);
    flags |= ((StackGetCapacity(stk) < 0)                          ? STACK_CAPACITY_NEG_ERROR     : 0);
    flags |= ((StackGetSize(stk) > StackGetCapacity(stk))          ? STACK_SIZE_G_CAPACITY_ERROR  : 0);

ON_CANARY_PROT(
    flags |= ((StackGetData(stk, -1) != CANARY ||
              StackGetData(stk, StackGetCapacity(stk)) != CANARY) ? STACK_CANARY_OVERWRITE_ERROR : 0);
)
    if (flags)
        return flags;

ON_HASH_PROT(
    flags |= (StackEvaluateHashData(stk) != StackGetHashData(stk))        ? STACK_HASH_DATA_DISPARITY_ERROR   : 0;
    flags |= (StackEvaluateHashStk(stk) != StackGetHashStk(stk))        ? STACK_HASH_STK_DISPARITY_ERROR   : 0;
)

    for (long int i = StackGetSize(stk); i < StackGetCapacity(stk); ++i)
        flags |= (StackGetData(stk, i) != POISON)                 ? STACK_DATA_POSION_ERROR      : 0;

    return flags;
}

ON_DEBUG(
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
)

void StackDump(Stack *stk, const char* func, const char* file, long int line)
{
ON_DEBUG(
    ASSERT(!isBadPtr(stk));
    int fdLogBuffer = getfdLogBuffer();

    dprintf(fdLogBuffer,"StackDump: from %s in %s(%ld)\n"
                        "Stack[%p]\n"
                        "Status: %s\n"
                        "%s at %s in %s(%ld): \n"
                        "{\n\tsize = %ld, \n"
                        "\tcapacity = %ld, \n"
                        "\tdata[%p] = \n\t{\n",
                        func, file, line,
                        stk,
                        StackGetStatus(stk),
                        StackGetName(stk), StackGetFuncname(stk), StackGetFilename(stk), StackGetLine(stk),
                        StackGetSize(stk),
                        StackGetCapacity(stk),
                        StackGetDataMem(stk));
ON_CANARY_PROT(
    dprintf(fdLogBuffer, "\t(c)\t[-1] = %ld\n",
                         StackGetDataMem(stk)[-1]);
)

    for (long int i = 0; i < StackGetSize(stk); ++i)
        dprintf(fdLogBuffer, "\t(*)\t[%ld] = %ld\n",
                             i, StackGetData(stk, i));

    for (long int i = StackGetSize(stk); i < StackGetCapacity(stk); ++i)
        dprintf(fdLogBuffer, "\t\t[%ld] = %ld\n",
                             i, StackGetData(stk, i));

ON_CANARY_PROT(
    dprintf(fdLogBuffer, "\t(c)\t[%ld] = %ld\n",
                         StackGetCapacity(stk) + 1,
                         StackGetData(stk, StackGetCapacity(stk)));
)

ON_HASH_PROT(
    dprintf(fdLogBuffer, "\thashData = %lx,\n"
                         "\thashStk  = %lx\n",
                         StackGetHashData(stk),
                         StackGetHashStk(stk));
)

    dprintf(fdLogBuffer, "\t}\n}\n\n");
)
}

ON_HASH_PROT(
void StackSetHashData(Stack *stk, uint64_t hashData)
{
    ASSERT(!isBadPtr(stk));
    stk->hashData = hashData;
}
void StackSetHashStk(Stack *stk, uint64_t hashStk)
{
    ASSERT(!isBadPtr(stk));
    stk->hashStk = hashStk;
}

uint64_t StackGetHashData(Stack *stk)
{
    ASSERT(!isBadPtr(stk));
    return stk->hashData;
}

uint64_t StackGetHashStk(Stack *stk)
{
    ASSERT(!isBadPtr(stk));
    return stk->hashStk;
}

uint64_t StackEvaluateHashData(Stack *stk)
{
    ASSERT(!isBadPtr(stk));
    static const uint64_t x = 677;
    uint64_t hashData = 0;
    for (long int i = 0; i < sizeof(stk->data) ON_CANARY_PROT(-1); ++i)
        hashData = hashData * x + ((char*)stk->data)[i];
    return hashData;
}

uint64_t StackEvaluateHashStk(Stack *stk)
{
    ASSERT(!isBadPtr(stk));
    static const uint64_t x = 677;
    const char *ptr = (const char*) stk;
    uint64_t hashStk = 0;
    for (size_t i = 0; i < sizeof(Stack) - 2 * sizeof(uint64_t); ++i)
        hashStk = hashStk * x + *ptr++;
    return hashStk;
}

void StackRehash(Stack *stk)
{
    ASSERT(!isBadPtr(stk));
    StackSetHashData(stk, StackEvaluateHashData(stk));
    StackSetHashStk (stk, StackEvaluateHashStk (stk));
}

)

bool isBadPtr(void *ptr)
{
    if (ptr == NULL)
        return false;
    int nullfd = open("/dev/random", O_WRONLY);
    bool res = write(nullfd, (char*) ptr, 1) < 0;
    close(nullfd);
    return res;
}
#endif
