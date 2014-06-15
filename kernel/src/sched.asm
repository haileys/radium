use32

global sched_begin
global sched_switch

extern current_task

%define USER_CODE (0x18 | 3)
%define USER_DATA (0x20 | 3)

%define task_fpu_state(task)     [(task) + 0]
%define task_esp(task)           [(task) + 512]
%define task_eip(task)           [(task) + 516]
%define task_page_dir_phys(task) [(task) + 528]

sched_begin:
    mov ax, USER_DATA
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov ecx, 0xffc00000 ; userland stack end
    mov edx, 0x10000000 ; task entry point
    sti
    sysexit

sched_next:
    mov eax, [current_task]
    ret

sched_switch:
    ; save old task state
    pusha
    mov eax, [current_task]
    fxsave task_fpu_state(eax)
    mov task_esp(eax), esp
    mov task_eip(eax), dword .return

    call sched_next

    mov ebx, task_page_dir_phys(eax)
    mov cr3, ebx

    fxrstor task_fpu_state(eax)
    mov esp, task_esp(eax)
    jmp task_eip(eax)
.return:
    popa
    ret
