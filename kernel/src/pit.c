#include "idt.h"
#include "io.h"
#include "panic.h"
#include "pit.h"

#define PIT_CLOCK_FREQUENCY 1193180

void
pit_set_frequency(uint32_t hz)
{
    uint32_t divisor = PIT_CLOCK_FREQUENCY / hz;

    if(divisor > 0xffff) {
        panic("frequency too low");
    }

    critical_begin();

    outb(0x43, 0x36);
    outb(0x40, divisor & 0xff);
    outb(0x40, (divisor >> 8) & 0xff);

    critical_end();
}
