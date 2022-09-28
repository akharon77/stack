#ifndef LOGH
#define LOGH

#define STACK_CTOR
#define STACK_PUSH
#define STACK_TOP
#define STACK_POP
#define STACK_RESIZE
#define STACK_REALLOC
#define STACK_CANARY_UPDATE
#define STACK_FILL_POISON
#define STACK_SET_DATA
#define STACK_SET_DATA_MEM
#define STACK_SET_SIZE
#define STACK_SET_CAPACITY
#define STACK_SET_LINE
#define STACK_SET_FILENAME
#define STACK_SET_FUNCNAME
#define STACK_SET_NAME
#define STACK_GET_NAME
#define STACK_GET_FUNCNAME
#define STACK_GET_FILENAME
#define STACK_GET_LINE
#define STACK_GET_DATA
#define STACK_GET_SIZE
#define STACK_GET_CAPACITY

#include "stack.h"

#define LOG(event, stk) log(#event, stk)

#define LOG_FILENAME "stack.log"

int getfdLogBuffer();
void closeLogBuffer();
void log(const char* event, const struct Stack *stk);

#endif
