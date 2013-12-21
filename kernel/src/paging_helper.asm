global paging_enabled
global set_page_directory

%define FL_PAGING_ENABLED 31

paging_enabled:
    mov eax, cr0
    shr eax, FL_PAGING_ENABLED
    ret

set_page_directory:
    mov eax, [esp + 4]
    mov cr3, eax

    mov eax, cr0
    or eax, (1 << FL_PAGING_ENABLED)
    mov cr0, eax

    ret
