use32

global _regdump
global _exit
global _yield
global _fork
global _wait

extern _main

section .crt0

%macro perform_syscall 1
    push ecx
    push edx
    mov ecx, esp
    mov edx, .ret
    mov eax, %1
    sysenter
.ret:
    pop edx
    pop ecx
%endmacro

start:
    call _main
    push eax
    call _exit

_regdump:
    perform_syscall 0
    ret

_exit:
    mov ebx, [esp+4] ; exit code
    perform_syscall 1
    ; unreachable

_yield:
    perform_syscall 2
    ret

_fork:
    perform_syscall 3
    ret

_wait:
    perform_syscall 4
    ret
