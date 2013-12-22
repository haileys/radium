global idt_init_asm
global idt_load
extern interrupts_register_isr
extern idtr
extern console_puts
extern panic

%macro register_isr 1
    push isr_%1
    push %1
    call interrupts_register_isr
    add esp, 8
%endmacro

%macro ack_irq 0
    mov al, 0x20
    out 0xa0, al
    out 0x20, al
%endmacro

section .text
idt_load:
    lidt [idtr]
    ret

idt_init_asm:
    register_isr 13
    register_isr 14
    register_isr 32
    ret

; general protection fault
isr_13:
    push .msg
    call panic
.msg db "general protection fault", 0

; page fault
isr_14:
    ; setup something that looks like a normal stack frame so panic can get a
    ; proper backtrace
    pop eax ; pop EFLAGS
    push ebp
    mov ebp, esp

    mov eax, cr2
    push eax
    push .msg
    call panic
    iret
.msg db "page fault at 0x%x", 0

; PIT irq
isr_32:
    pusha
    ack_irq
    popa
    iret
