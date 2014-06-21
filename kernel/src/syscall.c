#include <radium.h>

#include "console.h"
#include "panic.h"
#include "sched.h"
#include "syscall.h"
#include "task.h"
#include "util.h"

static uint32_t
syscall_regdump(registers_t* regs)
{
    printf("process %d register dump:\n", current_task->pid);
    printf("    eax=%x, ebx=%x, ecx=%x, edx=%x\n", regs->eax, regs->ebx, regs->ecx, regs->edx);
    printf("    esp=%x, ebp=%x, esi=%x, edi=%x\n", regs->esp, regs->ebp, regs->esi, regs->edi);

    return 0;
}

static uint32_t
syscall_exit(registers_t* regs)
{
    uint8_t code = regs->ebx & 0xff;

    if(current_task->pid == 1) {
        panic("init exited with status: %d", code);
    }

    // TODO - actually implement exit
    __asm__ volatile("cli\nhlt");

    return 0;
}

static uint32_t
syscall_yield(registers_t* regs)
{
    (void)regs;
    sched_switch();
    return 0;
}

static uint32_t
syscall_fork(registers_t* regs)
{
    (void)regs;

    task_t* new_task = task_fork();

    if(new_task) {
        // parent
        return new_task->pid;
    } else {
        // child
        return 0;
    }
}

typedef uint32_t(syscall_t)(registers_t*);

static syscall_t*
syscall_table[] = {
    [SYS_REGDUMP] = syscall_regdump,
    [SYS_EXIT]    = syscall_exit,
    [SYS_YIELD]   = syscall_yield,
    [SYS_FORK]    = syscall_fork,
};

void
syscall_dispatch(registers_t* regs)
{
    if(regs->eax > countof(syscall_table)) {
        regs->eax = -ENOSYS;
        return;
    }

    syscall_t* func = syscall_table[regs->eax];

    if(!func) {
        regs->eax = -ENOSYS;
        return;
    }

    current_task->syscall_registers = regs;

    regs->eax = func(regs);

    current_task->syscall_registers = NULL;
}
