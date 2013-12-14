#include <console.h>
#include <gdt.h>

void
kmain()
{
    gdt_init();
    console_init();

    console_puts("Hello world!\n");
    console_puts("Printing some stuff to the screen!\n");

    for(;;);
}
