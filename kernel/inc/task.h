#ifndef TASK_H
#define TASK_H

#include "types.h"
#include "syscall.h"

typedef struct {
    // 0x00
    uint16_t link;
    uint16_t _res_1;
    // 0x04
    uint32_t esp0;
    // 0x08
    uint16_t ss0;
    uint16_t _res_2;
    // 0x0c
    uint32_t esp1;
    // 0x10
    uint16_t ss1;
    uint16_t _res_3;
    // 0x14
    uint32_t esp2;
    // 0x18
    uint16_t ss2;
    uint16_t _res_4;
    // 0x1c
    uint32_t cr3;
    // 0x20
    uint32_t eip;
    // 0x24
    uint32_t eflags;
    // 0x28
    uint32_t eax;
    // 0x2c
    uint32_t ecx;
    // 0x30
    uint32_t edx;
    // 0x34
    uint32_t ebx;
    // 0x38
    uint32_t esp;
    // 0x3c
    uint32_t ebp;
    // 0x40
    uint32_t esi;
    // 0x44
    uint32_t edi;
    // 0x48
    uint16_t es;
    uint16_t _res_5;
    // 0x4c
    uint16_t cs;
    uint16_t _res_6;
    // 0x50
    uint16_t ss;
    uint16_t _res_7;
    // 0x54
    uint16_t ds;
    uint16_t _res_8;
    // 0x58
    uint16_t fs;
    uint16_t _res_9;
    // 0x5c
    uint16_t gs;
    uint16_t _res_10;
    // 0x60
    uint16_t ldtr;
    uint16_t _res_11;
    // 0x64
    uint16_t _res_12;
    uint16_t iopb;
    // 0x68
} __attribute__((packed))
tss_t;

// sched.asm refers to hardcoded offsets within this struct.
// make sure to change it when changing anything here.
typedef struct {
    /*   0 */ uint8_t fpu_state[512];
    /* 512 */ uint32_t esp;
    /* 516 */ uint32_t eip;
    /* 520 */ void* kernel_stack;
              // allocated within the kernel's identity-mapped region:
    /* 524 */ uint32_t* page_directory;
    /* 528 */ phys_t page_directory_phys;
    /* 532 */ registers_t* syscall_registers;
    /* 536 */ uint32_t pid;
    /* 540 */
}
task_t;

extern task_t*
current_task;

void
task_init();

void
task_boot_init(const char* init_bin, size_t size) __attribute__((noreturn));

task_t*
task_fork();

void
task_destroy(task_t* task);

#endif
