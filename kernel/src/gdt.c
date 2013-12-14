#include "gdt.h"
#include "types.h"

typedef struct {
    uint16_t limit_0_15;
    uint16_t base_0_15;
    uint8_t  base_16_23;
    uint8_t  access;
    uint8_t  limit_16_19_and_flags;
    uint8_t  base_24_31;
}
gdt_entry_t;

gdt_entry_t gdt[5];

static void
gdt_set_entry(gdt_selector_t sel, uint32_t base, uint32_t limit, gdt_privilege_t priv, gdt_type_t type)
{
    limit /= 4096;

    gdt_entry_t ent;
    ent.limit_0_15 = limit & 0xffff;
    ent.base_0_15 = base & 0xffff;
    ent.base_16_23 = (base >> 16) & 0xff;
    ent.access = (1 << 7)          // present
               | ((priv & 3) << 5) // privilege
               | (1 << 4)          // dunno lol
               | ((type & 1) << 3) // code/data
               | (1 << 2)          // data segments are always writable, code segments are always readable
               ;
    ent.limit_16_19_and_flags = ((limit >> 16) & 0x0f)
                              | (1 << 7) // 4 KiB granularity
                              | (1 << 6) // 32 bit segment;
                              ;
    ent.base_24_31 = (base >> 24) & 0xff;

    gdt[sel / 8] = ent;
}

void
gdt_reload();

void
gdt_init()
{
    gdt_set_entry(GDT_KERNEL_CODE, 0, 0xfffffffful, GDT_KERNEL, GDT_CODE);
    gdt_set_entry(GDT_KERNEL_DATA, 0, 0xfffffffful, GDT_KERNEL, GDT_DATA);
    gdt_set_entry(GDT_USER_CODE,   0, 0xfffffffful, GDT_USER,   GDT_CODE);
    gdt_set_entry(GDT_USER_DATA,   0, 0xfffffffful, GDT_USER,   GDT_DATA);

    gdt_reload();
}
