#include "syscall.h"
#include "console.h"

void
syscall_dispatch(registers_t* regs)
{
    printf("syscall! EAX = %x\n", regs->eax);
}
