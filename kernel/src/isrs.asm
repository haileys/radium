global idt_init_asm
global idt_load
extern interrupts_register_isr
extern idtr
extern console_puts

%macro isr_begin_handler 1
    jmp isr_%1_end
    isr_%1:
        pusha
%endmacro

%macro isr_end_handler 1
        mov al, 0x20
        out 0xa0, al
        out 0x20, al
        popa
        iret
    isr_%1_end:

    push isr_%1
    push %1
    call interrupts_register_isr
    add esp, 8
%endmacro

section .text
idt_load:
    lidt [idtr]
    ret

idt_init_asm:
    isr_begin_handler 13 ; GPF
        ; nop lol
    isr_end_handler 13

    isr_begin_handler 32 ; PIT
        push dot
        call console_puts
        add esp, 4
    isr_end_handler 32
ret

dot db ".", 0
