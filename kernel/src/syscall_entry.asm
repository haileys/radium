use32

extern printf

global syscall_entry
global syscall_init

%define IA32_SYSENTER_CS  0x174
%define IA32_SYSENTER_ESP 0x175
%define IA32_SYSENTER_EIP 0x176

%define KERNEL_CODE 0x08
%define KERNEL_DATA 0x10

syscall_init:
    xor edx, edx

    mov ecx, IA32_SYSENTER_CS
    mov eax, KERNEL_CODE
    wrmsr

    mov ecx, IA32_SYSENTER_ESP
    mov eax, 0x0ffffffc
    wrmsr

    mov ecx, IA32_SYSENTER_EIP
    mov eax, syscall_entry
    wrmsr

    ret

syscall_entry:
    pusha

    push .msg
    call printf
    add esp, 4

    popa
    sti
    sysexit

    .msg db "syscall!", 10, 0
