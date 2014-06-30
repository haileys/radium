global idt_init_asm
global idt_load
extern interrupts_register_isr
extern idtr
extern panic
extern sched_switch

%macro register_isr 1
    push isr_%1
    push %1
    call interrupts_register_isr
    add esp, 8
%endmacro

%macro ack_irq 0
    push ax
    mov al, 0x20
    out 0xa0, al
    out 0x20, al
    pop ax
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
    mov eax, cr2
    push eax
    push .msg
    call panic
    iret

    .msg db "page fault at 0x%x, error code: %x", 0
end_isr 14

; PIT irq
begin_isr 32
    ack_irq

    push ebp
    push dword 0
    push dword 0
    mov ebp, esp

    call sched_switch

    add esp, 8 ; fix up the fake stack frame we created
    pop ebp

    iret
end_isr 32

; keyboard irq
begin_isr 33
    ack_irq
    iret
end_isr 33

; spurious irq
begin_isr 39
    iret
end_isr 39

ret
