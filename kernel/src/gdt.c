#include "gdt.h"
#include "panic.h"
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

static gdt_entry_t
gdt[6];

volatile struct {
    uint16_t size;
    gdt_entry_t* offset;
} __attribute__((packed))
gdtr;

void
gdt_set_entry(gdt_selector_t sel, uint32_t base, uint32_t limit, gdt_privilege_t priv, gdt_type_t type)
{
    if(sel >= sizeof(gdt)) {
        panic("GDT overflow");
    }

    uint8_t flags = 1 << 6; // 32 bit segment

    // 1 MiB is the maximum segment size that can be expressed with 1 byte
    // granularity. If we need to express a size bigger than this, we need
    // to divide the size by 4 KiB and use 4 KiB granularity.
    if(limit >= (1 << 20)) {
        limit /= 4096;
        flags |= 1 << 7; // 4 KiB granularity
    }

    gdt_entry_t ent;
    ent.limit_0_15 = limit & 0xffff;
    ent.base_0_15 = base & 0xffff;
    ent.base_16_23 = (base >> 16) & 0xff;
    ent.access = (1 << 7)          // present
               | ((priv & 3) << 5) // privilege
               | (1 << 4)          // dunno lol
               | ((type & 1) << 3) // code/data
               | (1 << 1)          // data segments are always writable, code segments are always readable
               ;
    ent.limit_16_19_and_flags = ((limit >> 16) & 0x0f) | flags;
    ent.base_24_31 = (base >> 24) & 0xff;

    gdt[sel / sizeof(gdt_entry_t)] = ent;
}

void
gdt_reload();

void
gdt_init()
{
    gdtr.size = sizeof(gdt) - 1;
    gdtr.offset = gdt;

    gdt_set_entry(GDT_KERNEL_CODE, 0, 0xfffffffful, GDT_KERNEL, GDT_CODE);
    gdt_set_entry(GDT_KERNEL_DATA, 0, 0xfffffffful, GDT_KERNEL, GDT_DATA);
    gdt_set_entry(GDT_USER_CODE,   0, 0xfffffffful, GDT_USER,   GDT_CODE);
    gdt_set_entry(GDT_USER_DATA,   0, 0xfffffffful, GDT_USER,   GDT_DATA);

    gdt_reload();
}
