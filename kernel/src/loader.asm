global loader
global end_of_image

extern kmain

section .multiboot_header
align 4
multiboot_header:
    dd 0x1badb002        ; magic
    dd 3                 ; flags
    dd -(0x1badb002 + 3) ; checksum = -(flags + magic)

section .text
align 4
loader:
    mov esp, stack
    push dword 0
    push dword 0
    mov ebp, esp

    push eax ; multiboot magic number
    push ebx ; pointer to multiboot struct

    fninit
    mov eax, cr0
    or eax, 1 << 5  ; FPU NE bit
    mov cr0, eax

    call kmain

section .bss
align 4
resb 65536
stack:

section .end_of_image
end_of_image:
