global panic_print_backtrace
global panic_symbols

extern printf
extern vprintf
extern panic_print_backtrace_item

section .rodata

panic_backtrace_item db "  [<0x%x>]", 10, 0

panic_symbols:
    db '@@@ PANIC SYMBOL TABLE @@@'
    times 65536 - ($ - panic_symbols) db 0

section .text
panic_print_backtrace:
    mov esp, ebp
    pop ebp
    cmp dword [esp], 0
    je .end_loop
    call panic_print_backtrace_item
    jmp panic_print_backtrace
.end_loop:
    hlt
