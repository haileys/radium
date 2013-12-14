#include <console.h>
#include <gdt.h>
#include <idt.h>
#include <io.h>
#include <types.h>

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

    printf("Interrupt no. #%d points to 0x%x\n", interrupt_no, handler);
}

void
interrupts_set_callable_from_user_mode(uint8_t interrupt_no)
{
    idt[interrupt_no].type_attr |= 3 << 5;
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
    interrupts_disable();

    remap_irqs();
    idt_init_asm();

    idtr.size = sizeof(idt) - 1;
    idtr.offset = idt;
    idt_load();

    interrupts_enable();
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
