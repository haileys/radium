global sched_begin_multitasking_iret

%define USER_CODE (0x18 | 3)
%define USER_DATA (0x20 | 3)

sched_begin_multitasking_iret:
    push dword USER_DATA  ; ss
    push dword 0          ; esp
    pushf                 ; eflags
    push dword USER_CODE  ; cs
    push dword 0x10000000 ; esp
    mov ax, USER_DATA
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    iret
