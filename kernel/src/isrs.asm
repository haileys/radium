global idt_init_asm
global idt_load
extern interrupts_register_isr
extern idtr
extern console_puts
extern panic
extern printf

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

%macro begin_isr 1
    jmp isr_%1.end
    isr_%1:
%endmacro

%macro end_isr 1
    .end:
    register_isr %1
%endmacro

%macro generic_exception 2
    begin_isr %1
        push .msg
        call panic
        .msg db %2, 0
    end_isr %1
%endmacro

section .text
idt_load:
    lidt [idtr]
    ret

idt_init_asm:

generic_exception 0,  "divide by zero"
generic_exception 1,  "debug"
generic_exception 2,  "non-maskable interrupt"
generic_exception 3,  "breakpoint"
generic_exception 4,  "overflow"
generic_exception 5,  "bound range exceeded"
generic_exception 6,  "invalid opcode"
generic_exception 7,  "device not available"
generic_exception 8,  "double fault"
generic_exception 10, "invalid tss"
generic_exception 11, "segment not present"
generic_exception 12, "stack segment fault"
generic_exception 13, "general protection fault"

; page fault
begin_isr 14
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
end_isr 14

; PIT irq
begin_isr 32
    pusha
    push .msg
    call printf
    add esp, 4
    ack_irq
    popa
    iret
    .msg db ".", 0
end_isr 32

; keyboard irq
begin_isr 33
    ack_irq
    iret
end_isr 33

ret
