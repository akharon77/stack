#ifndef STACKH
#define STACKH

#define MAX(a, b) ((a) > (b) ? (a) : (b))

extern const long int POISON;

struct Stack
{
    long int canary1;
    long int size;
    long int capacity;
    Elem *data;
#ifdef _DEBUG
    long int line;
    const char* filename;
    const char* name;
#endif
    long int canary2;
};

void StackCtor(Stack *stk, int cap);

void StackResize(Stack *stk, int nsize);

#endif
