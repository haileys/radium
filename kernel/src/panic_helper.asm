global panic
global panic_symbols

extern printf
extern panic_print_backtrace_item

section .rodata

panic_msg db 10, "panic: %s", 10, 0
panic_backtrace_item db "  [<0x%x>]", 10, 0

panic_symbols:
    db '@@@ PANIC SYMBOL TABLE @@@'
    times 65536 - ($ - panic_symbols) db 0

section .text
panic:
    push ebp
    mov ebp, esp
    cli
    push dword [esp+8]
    push panic_msg
    call printf
    add esp, 8
.backtrace_loop:
    mov esp, ebp
    pop ebp
    cmp dword [esp], 0
    je .end_loop
    push dword [esp]
    call panic_print_backtrace_item
    add esp, 4
    jmp .backtrace_loop
.end_loop:
    hlt
