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

void
console_log(const char* str)
{
    uint32_t len = 0;

    for(const char* p = str; *p; p++) {
        len++;
    }

    _syscall2(SYS_CONSOLE_LOG, (uint32_t)str, len);
}
