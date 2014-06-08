use32
org 0x10000000

start:
    mov edx, .syscall_return_address
    sysenter
.syscall_return_address:
    ; burn some time the hacky way:
    mov ecx, 0x200000
.lp:
    nop
    loop .lp
    jmp start
