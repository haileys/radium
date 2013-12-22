#include "console.h"
#include "paging.h"
#include "panic.h"
#include "string.h"

static phys_t
next_free_page;

#define FL_PAGING_ENABLED (1 << 31)

static bool
paging_enabled()
{
    uint32_t cr0;
    __asm__("mov %0, cr0" : "=r"(cr0));
    return !!(cr0 & FL_PAGING_ENABLED);
}

void
set_page_directory(phys_t page_directory)
{
    uint32_t cr0;

    __asm__("mov cr3, %0" :: "r"(page_directory));

    __asm__("mov %0, cr0" : "=r"(cr0));
    cr0 |= FL_PAGING_ENABLED;
    __asm__("mov cr0, %0" :: "r"(cr0) : "memory");
}

phys_t
page_alloc()
{
    phys_t page = next_free_page;

    if(paging_enabled()) {
        panic("TODO - make page_alloc work when paging is enabled");
    } else {
        next_free_page = *(phys_t*)page;
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

static void
invlpg(virt_t virt)
{
    __asm__ volatile("invlpg [%0]" :: "r"(virt) : "memory");
}

void
page_map(virt_t virt_page, phys_t phys_page, int flags)
{
    // page directories are always recursively mapped into themselves:
    uint32_t* current_page_directory = (uint32_t*)0xfffff000;

    size_t page_dir_i = virt_page / 4096 / 1024;
    size_t page_tab_i = virt_page / 4096 % 1024;

    uint32_t* page_table = (uint32_t*)(0xffc00000 + page_dir_i * 4096);

    uint32_t pd_entry = current_page_directory[page_dir_i];
    if(!(pd_entry & PE_PRESENT)) {
        current_page_directory[page_dir_i] = page_alloc() | (flags & PE_FLAG_MASK);
        invlpg((virt_t)page_table);
    }

    page_table[page_tab_i] = (phys_page & PE_ADDR_MASK) | (flags & PE_FLAG_MASK);
    invlpg((virt_t)virt_page);
}
