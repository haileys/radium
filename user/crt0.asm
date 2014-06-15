use32

global regdump
extern main

section .crt0

start:
    call main
    push eax
    call exit

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

exit:
    mov eax, 1     ; syscall number 1
    mov ebx, [esp+4] ; pass exit code in EBX
    ; don't bother setting ECX and EDX because this syscall never returns
    sysenter
