#include "console.h"
#include "gdt.h"
#include "idt.h"
#include "kernel_page.h"
#include "multiboot.h"
#include "paging.h"
#include "panic.h"
#include "pit.h"
#include "sched.h"
#include "string.h"
#include "syscall.h"
#include "task.h"
#include "types.h"

static multiboot_info_t* mb;

static multiboot_module_t*
find_module(const char* name)
{
    multiboot_module_t* mods = (void*)mb->mods_addr;

    for(size_t i = 0; i < mb->mods_count; i++) {
        if(streq((const char*)mods[i].cmdline, name)) {
            return &mods[i];
        }
    }

    return NULL;
}

void
kmain(multiboot_info_t* mb_, uint32_t magic)
{
    (void)magic;
    mb = mb_;

    console_init();

    printf("Radium booting from %s.\n", (const char*)mb->boot_loader_name);

    for(size_t i = 0; i < mb->mods_count; i++) {
        multiboot_module_t* mods = (void*)mb->mods_addr;
        paging_set_allocatable_start(mods[i].mod_end);
    }

    gdt_init();
    idt_init();
    pit_set_frequency(100);
    paging_init(mb);
    task_init();
    syscall_init();

    multiboot_module_t* mod = find_module("/init.bin");

    task_init_load_text((const char*)mod->mod_start, mod->mod_end - mod->mod_start);

    sched_begin_multitasking();

    while(1) {
        __asm__ volatile("hlt");
    }
}
