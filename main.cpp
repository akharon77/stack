#define Elem long int
#include "stack.h"

int main()
{
    Stack stk1;
    Elem *ptr = (Elem*) calloc(2, sizeof(Elem));
    StackCtor(&stk1, 10);
    stk1.data = ptr;
    for (int i = 0; i < 10; ++i)
        StackPush(&stk1, i);
    for (int i = 0; i < 5; ++i)
        printf("%d\n", StackPop(&stk1));
    for (int i = 0; i < 10; ++i)
        StackPush(&stk1, i);
    for (int i = 0; i < 100; ++i)
        StackPush(&stk1, i);
    for (int i = 0; i < 100; ++i)
        printf("%d\n", StackPop(&stk1));
    for (int i = 0; i < 100; ++i)
        StackPush(&stk1, i);
    stk1.data[10] = 1;
    StackDtor(&stk1);
    free(ptr);
    return 0;
}
