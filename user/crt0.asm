use32

global regdump
extern main

section .crt0

start:
    call main
    jmp $

regdump:
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
