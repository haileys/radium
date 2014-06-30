#include "console.h"
#include "idt.h"
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

static void
invlpg(virt_t virt)
{
    __asm__ volatile("invlpg [%0]" :: "r"(virt) : "memory");
}

phys_t
page_alloc()
{
    phys_t page = next_free_page;

    if(paging_enabled()) {
        phys_t* temp_mapping = page_temp_map(page);
        next_free_page = *temp_mapping;
        page_temp_unmap();
    } else {
        next_free_page = *(phys_t*)page;
    }

    return page;
}

void
page_free(phys_t addr)
{
    if(paging_enabled()) {
        phys_t* temp_mapping = page_temp_map(addr);
        *temp_mapping = next_free_page;
        page_temp_unmap();
    } else {
        *(phys_t*)addr = next_free_page;
    }

    next_free_page = addr;
}

void
page_map(virt_t virt_page, phys_t phys_page, page_flags_t flags)
{
    // page directories are always recursively mapped into themselves:
    uint32_t* current_page_directory = (uint32_t*)CURRENT_PAGE_DIRECTORY;

    size_t page_dir_i = (virt_page / 4096) / 1024;
    size_t page_tab_i = (virt_page / 4096) % 1024;

    uint32_t* page_table = (uint32_t*)(CURRENT_PAGE_TABLE_BASE + page_dir_i * PAGE_SIZE);

    uint32_t pd_entry = current_page_directory[page_dir_i];
    if(!(pd_entry & PE_PRESENT)) {
        current_page_directory[page_dir_i] = page_alloc() | (flags & PE_FLAG_MASK);
        invlpg((virt_t)page_table);
        memset(page_table, 0, 4096);
    }

    page_table[page_tab_i] = (phys_page & PE_ADDR_MASK) | (flags & PE_FLAG_MASK);
    invlpg((virt_t)virt_page);
}

void
page_unmap(virt_t virt_page)
{
    page_map(virt_page, 0, 0);
}

phys_t
virt_to_phys(virt_t virt)
{
    uint32_t* current_page_directory = (uint32_t*)CURRENT_PAGE_DIRECTORY;

    size_t page_dir_i = (virt / 4096) / 1024;
    size_t page_tab_i = (virt / 4096) % 1024;
    size_t page_offset = virt % 4096;

    if(!(current_page_directory[page_dir_i] & PE_PRESENT)) {
        return 0;
    }

    uint32_t* page_table = (uint32_t*)(CURRENT_PAGE_TABLE_BASE + page_dir_i * PAGE_SIZE);

    uint32_t page_tab_entry = page_table[page_tab_i];

    if(!(page_tab_entry & PE_PRESENT)) {
        return 0;
    }

    return (page_tab_entry & PE_ADDR_MASK) + page_offset;
}

static uint32_t* const
temp_page_entry = (void*)CURRENT_PAGE_TABLE_BASE;

static uint32_t
old_null_page;

void*
page_temp_map(phys_t phys_page)
{
    old_null_page = *temp_page_entry;
    *temp_page_entry = phys_page | PE_PRESENT | PE_READ_WRITE;
    invlpg(0);
    return NULL; // we map in the temp page at NULL - todo fix...
}

void
page_temp_unmap()
{
    *temp_page_entry = old_null_page;
    invlpg(0);
}

bool
page_is_user_mapped(virt_t virt)
{
    uint32_t dir_i = virt / (1024 * PAGE_SIZE);
    uint32_t base_i = (virt / PAGE_SIZE);

    uint32_t* page_directory = (uint32_t*)CURRENT_PAGE_DIRECTORY;

    if(!(page_directory[dir_i] & (PE_PRESENT | PE_USER))) {
        return false;
    }

    uint32_t* page_table_base = (uint32_t*)CURRENT_PAGE_TABLE_BASE;

    if(!(page_table_base[base_i] & (PE_PRESENT | PE_USER))) {
        return false;
    }

    return true;
}
