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
