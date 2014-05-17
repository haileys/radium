#include "console.h"
#include "gdt.h"
#include "paging.h"
#include "string.h"
#include "task.h"
#include "util.h"

static_assert(tss_t_is_0x68_bytes_long, sizeof(tss_t) == 0x68);

static tss_t
tss;

void
task_init()
{
    printf("setting up TSS at 0x%x\n", &tss);

    gdt_set_entry(GDT_TSS, (uint32_t)&tss, sizeof(tss), GDT_KERNEL, GDT_CODE);
    gdt_reload();

    tss.ss0 = GDT_KERNEL_DATA;
    tss.esp0 = KERNEL_STACK_END;

    // pointer to the IO permission bitmap is beyond the end of the segment
    tss.iopb = sizeof(tss);

    // __asm__ volatile("ltr ax" :: "a"((uint16_t)GDT_TSS));
}
}
