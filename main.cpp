#define _DEBUG
#include "stack.h"


int main()
{
    Stack stk;
    StackCtor(&stk, 10);
    StackPush(&stk, 1);
    return 0;
}
