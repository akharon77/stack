#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include "log.h"
#include "stack.h"

static int fdLogBuffer = -1;

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

void log(const char* event, const Stack *stk)
{
    ON_DEBUG(
        dprintf(fdLogBuffer, "[%s]\n", event);
        StackDump(stk);
    )
}
