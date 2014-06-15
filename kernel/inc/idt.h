#ifndef IDT_H
#define IDT_H

void
idt_init();

void
interrupts_disable();

void
interrupts_enable();

void
critical_begin();

void
critical_end();

void
critical_reset();

#endif
