use32

global sched_begin_multitasking

%define USER_CODE (0x18 | 3)
%define USER_DATA (0x20 | 3)

sched_begin_multitasking:
    mov ax, USER_DATA
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    xor ecx, ecx        ; let userland set up its own stack
    mov edx, 0x10000000 ; task entry point
    sysexit
