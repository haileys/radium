use32

global __syscall0
global __syscall1
global __syscall2
global __syscall3

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
    mov eax, [esp+0+4]
    perform_syscall
    ret

__syscall1:
    push ebx
    mov ebx, [esp+4+8]
    mov eax, [esp+4+4]
    perform_syscall
    pop ebx
    ret

__syscall2:
    push ebx
    push edi
    mov edi, [esp+8+12]
    mov ebx, [esp+8+8]
    mov eax, [esp+8+4]
    perform_syscall
    pop edi
    pop ebx
    ret

__syscall3:
    push ebx
    push edi
    push esi
    mov esi, [esp+12+16]
    mov edi, [esp+12+12]
    mov ebx, [esp+12+8]
    mov eax, [esp+12+4]
    perform_syscall
    pop esi
    pop edi
    pop ebx
    ret
