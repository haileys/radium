use32

global __syscall0
global __syscall1

extern _main
extern _exit

section .crt0

%macro perform_syscall 0
    push ecx
    push edx
    mov ecx, esp
    mov edx, .ret
    sysenter
.ret:
    pop edx
    pop ecx
%endmacro

start:
    call _main
    push eax
    call _exit

__syscall0:
    mov eax, [esp+4]
    perform_syscall
    ret

__syscall1:
    push ebx
    mov ebx, [esp+12]
    mov eax, [esp+8]
    perform_syscall
    pop ebx
    ret
