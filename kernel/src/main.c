#include "console.h"
#include "gdt.h"
#include "idt.h"
#include "multiboot.h"
#include "paging.h"
#include "panic.h"
#include "types.h"

void
kmain(multiboot_info_t* mb, uint32_t magic)
{
    (void)magic;

    interrupts_disable();

    console_init();

    printf("Radium booting from %s.\n", (const char*)mb->boot_loader_name);

    for(size_t i = 0; i < mb->mods_count; i++) {
        multiboot_module_t* mods = (void*)mb->mods_addr;
        paging_set_allocatable_start(mods[i].mod_end);
    }

    gdt_init();
    idt_init();
    paging_init(mb);

    printf("Booted.\n");

    interrupts_enable();

    while(1) {
        __asm__ volatile("hlt");
    }
}
