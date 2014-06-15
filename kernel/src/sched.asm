use32

global sched_begin

%define USER_CODE (0x18 | 3)
%define USER_DATA (0x20 | 3)

sched_begin:
    mov ax, USER_DATA
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov ecx, 0xffc00000 ; userland stack end
    mov edx, 0x10000000 ; task entry point
    sti
    sysexit
