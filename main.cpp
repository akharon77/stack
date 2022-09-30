#define Elem long int
#include "stack.h"

int main()
{
    Stack stk1;
    StackCtor(&stk1, 10);
    stk1.data[0] = 1;
    for (int i = 0; i < 10; ++i)
        StackPush(&stk1, i);
    for (int i = 0; i < 5; ++i)
        printf("%d\n", StackPop(&stk1));
    for (int i = 0; i < 10; ++i)
        StackPush(&stk1, i);
    for (int i = 0; i < 15; ++i)
        printf("%d\n", StackPop(&stk1));
    for (int i = 0; i < 100; ++i)
        StackPush(&stk1, i);
    for (int i = 0; i < 100; ++i)
        printf("%d\n", StackPop(&stk1));
    StackDtor(&stk1);
    return 0;
}
