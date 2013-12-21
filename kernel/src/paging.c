#include "console.h"
#include "paging.h"
#include "panic.h"
#include "string.h"

static phys_t
next_free_page;

static bool
paging_enabled()
{
    uint32_t cr0;
    __asm__("mov %0, cr0" : "=r"(cr0));
    return !!(cr0 & (1 << 31));
}

void
set_page_directory(phys_t page_directory)
{
    __asm__("mov cr3, %0" :: "r"(page_directory));
}

phys_t
page_alloc()
{
    phys_t page = next_free_page;

    if(paging_enabled()) {
        panic("TODO - make page_alloc work when paging is enabled");
    } else {
        next_free_page = *(phys_t*)page;
        memset((void*)page, 0, PAGE_SIZE);
    }

    return page;
}

void
page_free(phys_t addr)
{
    if(paging_enabled()) {
        panic("TODO - make page_free work when paging is enabled");
    } else {
        *(phys_t*)addr = next_free_page;
    }

    next_free_page = addr;
}
