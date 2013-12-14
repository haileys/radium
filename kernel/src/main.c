#include <console.h>
#include <idt.h>
#include <gdt.h>

void
kmain()
{
    console_init();

    gdt_init();
    idt_init();

    console_puts("Hello world!\n");
    console_puts("Printing some stuff to the screen! ");

    for(;;);
}
