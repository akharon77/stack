#ifndef STACKH
#define STACKH

#define MAX(a, b) ((a) > (b) ? (a) : (b))

extern const long int POISON;

struct Stack
{
    int canary1;
    int size;
    int capacity;
    Elem *data;
    int canary2;
};

void StackCtor(Stack *stk, int cap);

void StackResize(Stack *stk, int nsize);

#endif
