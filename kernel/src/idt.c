#include "console.h"
#include "gdt.h"
#include "idt.h"
#include "io.h"
#include "types.h"

typedef struct {
    uint16_t offset_0_15;
    uint16_t segment;
    uint8_t zero;
    uint8_t type_attr;
    uint16_t offset_16_31;
}
idt_entry_t;

static idt_entry_t
idt[256];

volatile struct {
    uint16_t size;
    idt_entry_t* offset;
} __attribute__((packed))
idtr;

void
interrupts_register_isr(uint8_t interrupt_no, uint32_t handler)
{
    idt_entry_t ent;

    ent.offset_0_15 = handler & 0xffff;
    ent.segment = GDT_KERNEL_CODE;
    ent.zero = 0;
    ent.type_attr = (1 << 7) // present
                  | 0xe      // interrupt gate
                  ;
    ent.offset_16_31 = (handler >> 16) & 0xffff;

    idt[interrupt_no] = ent;
}

static void
remap_irqs()
{
    // remap IRQ table
    outb(0x20, 0x11);
    outb(0xa0, 0x11);
    outb(0x21, 0x20);
    outb(0xa1, 0x28);
    outb(0x21, 0x04);
    outb(0xa1, 0x02);
    outb(0x21, 0x01);
    outb(0xa1, 0x01);
    outb(0x21, 0x00);
    outb(0xa1, 0x00);
}

void
idt_init_asm();

void
idt_load();

void
idt_init()
{
    remap_irqs();
    idt_init_asm();

    idtr.size = sizeof(idt) - 1;
    idtr.offset = idt;
    idt_load();
}

void
interrupts_disable()
{
    __asm__ volatile("cli");
}

void
interrupts_enable()
{
    __asm__ volatile("sti");
}

static uint32_t
critical_nesting = 0;

void
critical_begin()
{
    interrupts_disable();
    critical_nesting++;
}

void
critical_end()
{
    if(--critical_nesting == 0) {
        interrupts_enable();
    }
}
