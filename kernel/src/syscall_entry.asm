use32

extern syscall_dispatch

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

    push dword 0
    push dword 0
    mov ebp, esp

    lea eax, [esp + 8]
    push eax
    call syscall_dispatch
    add esp, 12 ; 4 bytes for the argument we passed to syscall_dispatch, and
                ; another 8 to compensate for the fake stack frame we pushed.

    popa

    ; STI apparently waits one instruction before enabling interrupts, so
    ; despite how it appears, this return sequence should be race-free.
    sti
    sysexit
