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
#define ON_HASH_PROT(...) __VA_ARGS__
#else
#define ON_HASH_PROT(...)
#endif

#ifdef _DEBUG
#define ON_DEBUG(...) __VA_ARGS__
#else
#define ON_DEBUG(...)
#endif

extern const long int POISON;
extern const long int CANARY;

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

#define StackCtor(stk, capacity)               \
StackCtor_(                                    \
           stk,                                \
           capacity                            \
           ON_DEBUG(, __LINE__)                \
           ON_DEBUG(, __FILE__)                \
           ON_DEBUG(, __FUNCTION__)            \
           ON_DEBUG(, #stk + (#stk[0] == '&')) \
          )

void StackCtor_(
                Stack *stk,
                long int capacity 
                ON_DEBUG(, long int line)
                ON_DEBUG(, const char* filename)
                ON_DEBUG(, const char* funcname)
                ON_DEBUG(, const char* name)
               );

void StackDtor(Stack *stk);
void StackPush(Stack *stk, Elem el);
Elem StackTop(Stack *stk);
Elem StackPop(Stack *stk);
void StackResize(Stack *stk, long int size);
void StackRealloc(Stack *stk, long int capacity);
void StackSetSize(Stack *stk, long int size);
void StackSetCapacity(Stack *stk, long int capacity);
void StackSetDataMem(Stack *stk, Elem *data);
void StackSetData(Stack *stk, long int ind, Elem value);
void StackFillPoison(Stack *stk, long int l, long int r);
long int StackGetSize(Stack *stk);
long int StackGetCapacity(Stack *stk);
long int StackGetData(Stack *stk, long int ind);
long int StackGetCoeff(Stack *stk);

ON_CANARY_PROT(void StackCanaryUpdate(Stack *stk);)

ON_DEBUG(
    void StackDump(const Stack *stk);
    long int StackGetLine(Stack *stk);
    const char* StackGetName(Stack *stk);
    const char* StackGetFuncname(Stack *stk);
    const char* StackGetFilename(Stack *stk);
    void StackSetLine(Stack *stk, long int line);
    void StackSetName(Stack *stk, const char *name);
    void StackSetFuncname(Stack *stk, const char *funcname);
    void StackSetFilename(Stack *stk, const char *filename);
)

#endif
