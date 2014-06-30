use32

global _regdump
global _exit
global _yield
global _fork

extern _main

section .crt0

start:
    call _main
    push eax
    call _exit

_regdump:
    push ecx
    push edx
    mov ecx, esp
    mov edx, .ret
    mov eax, 0 ; syscall number 0
    sysenter
.ret:
    pop edx
    pop ecx
    ret

_exit:
    mov eax, 1     ; syscall number 1
    mov ebx, [esp+4] ; pass exit code in EBX
    ; don't bother setting ECX and EDX because this syscall never returns
    sysenter

_yield:
    push ecx
    push edx
    mov ecx, esp
    mov edx, .ret
    mov eax, 2 ; syscall number 2
    sysenter
.ret:
    pop edx
    pop ecx
    ret

_fork:
    push ecx
    push edx
    mov ecx, esp
    mov edx, .ret
    mov eax, 3 ; syscall number 3
    sysenter
.ret:
    pop edx
    pop ecx
    ret
