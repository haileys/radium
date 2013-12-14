#include <console.h>

void
kmain()
{
    console_init();

    console_puts("Hello world!\n");
    console_puts("Printing some stuff to the screen!\n");

    for(;;);
}
