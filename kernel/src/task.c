#include "console.h"
#include "gdt.h"
#include "task.h"

static tss_t
tss;

void
task_init()
{
    printf("setting up TSS at 0x%x\n", &tss);

    gdt_set_entry(GDT_TSS, (uint32_t)&tss, sizeof(tss), GDT_KERNEL, GDT_CODE);
    gdt_reload();

    __asm__ volatile("ltr ax" :: "a"((uint16_t)GDT_TSS));
}
