#include <radium.h>

#include "console.h"
#include "panic.h"
#include "sched.h"
#include "syscall.h"
#include "task.h"
#include "util.h"

#define REG_VECTOR(regs) ((regs)->eax)
#define REG_RETURN(regs) ((regs)->eax)

#define REG_ARG1(regs) ((regs)->ebx)
#define REG_ARG2(regs) ((regs)->edi)
#define REG_ARG3(regs) ((regs)->esi)

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
    uint8_t status = REG_ARG1(regs) & 0xff;

    if(current_task->pid == 1) {
        panic("init exited with status: %d", status);
    }

    task_kill(current_task, status);

    // goodbye!
    sched_switch();

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

static uint32_t
syscall_wait(registers_t* regs)
{
    (void)regs;

again:
    if(current_task->wait_queue.live.head) {
        task_t* child = current_task->wait_queue.live.head;
        current_task->wait_queue.live.head = child->wait_queue.dead.next;

        uint32_t child_pid = child->pid;

        task_destroy(child);

        return child_pid;
    }

    current_task->state = TASK_BLOCK_WAIT;
    sched_switch();
    goto again;
}

typedef uint32_t(syscall_t)(registers_t*);

static syscall_t*
syscall_table[] = {
    [SYS_REGDUMP] = syscall_regdump,
    [SYS_EXIT]    = syscall_exit,
    [SYS_YIELD]   = syscall_yield,
    [SYS_FORK]    = syscall_fork,
    [SYS_WAIT]    = syscall_wait,
};

void
syscall_dispatch(registers_t* regs)
{
    if(REG_VECTOR(regs) > countof(syscall_table)) {
        REG_RETURN(regs) = -ENOSYS;
        return;
    }

    syscall_t* func = syscall_table[regs->eax];

    if(!func) {
        REG_RETURN(regs) = -ENOSYS;
        return;
    }

    current_task->syscall_registers = regs;

    REG_RETURN(regs) = func(regs);

    current_task->syscall_registers = NULL;
}
