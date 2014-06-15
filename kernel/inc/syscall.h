#ifndef SYSCALL_H
#define SYSCALL_H

#include "types.h"

typedef struct {
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
}
registers_t;

void
syscall_init();

#endif
