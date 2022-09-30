#include "stack.h"

int main()
{
    Stack stkLol;
    StackCtor(&stkLol, 10);
    StackPush(&stkLol, 1);
    stkLol.size = -329;
    StackPop(&stkLol);
    StackDtor(&stkLol);
    return 0;
}
