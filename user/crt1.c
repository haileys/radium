#include <radium.h>
#include "crt.h"

void
regdump()
{
    _syscall0(SYS_REGDUMP);
}

void
exit(int status)
{
    _syscall1(SYS_EXIT, status);
}

void
yield()
{
    _syscall0(SYS_YIELD);
}

uint32_t
fork()
{
    return _syscall0(SYS_FORK);
}

uint32_t
wait()
{
    return _syscall0(SYS_WAIT);
}
