#include <console.h>
#include <gdt.h>
#include <idt.h>
#include <multiboot.h>
#include <panic.h>
#include <types.h>

void
kmain(multiboot_info_t* mb, uint32_t magic)
{
    (void)magic;

    console_init();

    gdt_init();
    idt_init();

    printf("Radium booted by %s.\n", (const char*)mb->boot_loader_name);

    panic("test panic");

    for(;;);
}
