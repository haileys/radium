global gdt_reload

section .text
gdt_reload:
    jmp 0x08:.flush_cs
.flush_cs:
    mov eax, 0x10
    mov ds, eax
    mov es, eax
    mov fs, eax
    mov gs, eax
    mov ss, eax
    ret
