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

    task_t init_task;
    task_new(&init_task);

    multiboot_module_t* mod = find_module("/init.bin");

    set_page_directory(init_task.page_directory_phys);

    for(size_t i = 0; i < mod->mod_end - mod->mod_start; i += PAGE_SIZE) {
        phys_t page = page_alloc();
        page_map(USER_BEGIN + i, page, PE_PRESENT | PE_USER);
        size_t size = mod->mod_end - i;
        if(size > PAGE_SIZE) {
            size = PAGE_SIZE;
        }
        memcpy((void*)(USER_BEGIN + i), (void*)(mod->mod_start + i), size);
    }

    sched_begin_multitasking();

    while(1) {
        __asm__ volatile("hlt");
    }
}
