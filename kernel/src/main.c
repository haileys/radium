#include <console.h>
#include <gdt.h>
#include <idt.h>
#include <multiboot.h>
#include <paging.h>
#include <panic.h>
#include <types.h>

void
kmain(multiboot_info_t* mb, uint32_t magic)
{
    (void)magic;

    interrupts_disable();

    console_init();

    printf("Radium booting from %s.\n", (const char*)mb->boot_loader_name);

    gdt_init();
    idt_init();
    paging_init(mb);

    printf("Booted.\n");

    interrupts_enable();

    *(int*)0xdeadbeef = 0;

    for(;;);
}
