#include <console.h>
#include <multiboot.h>
#include <paging.h>
#include <panic.h>
#include <string.h>

#define PAGE_SIZE 4096

#define PD_PRESENT    (1 << 0)
#define PD_READ_WRITE (1 << 1)
#define PD_USER       (1 << 2)
#define PD_ADDR_MASK  ~(PAGE_SIZE - 1)

#define PT_PRESENT    (1 << 0)
#define PT_READ_WRITE (1 << 1)
#define PT_USER       (1 << 2)
#define PT_ADDR_MASK  ~(PAGE_SIZE - 1)

static size_t
kernel_end;

static phys_t
next_free_page;

static size_t
register_available_memory_region(multiboot_memory_map_t* region)
{
    size_t pages_registered = 0;

    for(size_t offset = 0; offset + PAGE_SIZE <= region->len; offset += PAGE_SIZE) {
        phys_t addr = region->addr + offset;

        if(addr < kernel_end) {
            // don't put memory before the end of the kernel in the free list
            continue;
        }

        page_free_raw(addr);
        pages_registered++;
    }

    return pages_registered;
}

static size_t
register_available_memory(multiboot_info_t* mb)
{
    multiboot_memory_map_t* mmap = (void*)mb->mmap_addr;
    size_t pages_registered = 0;

    for(size_t i = 0; i < mb->mmap_length / sizeof(multiboot_memory_map_t); i++) {
        if(mmap[i].type == MULTIBOOT_MEMORY_AVAILABLE) {
            pages_registered += register_available_memory_region(mmap + i);
        }
    }

    return pages_registered;
}

static void
identity_map_kernel()
{
    uint32_t* page_directory = (uint32_t*)page_alloc_raw();

    // we start looping from PAGE_SIZE in order to leave the null page unmapped
    // accessing it will cause a GPF.
    for(phys_t addr = PAGE_SIZE; addr <= kernel_end; addr += PAGE_SIZE) {
        size_t page_dir_i = addr / 4096 / 1024;
        size_t page_tab_i = addr / 4096 % 1024;
        uint32_t page_dir_ent = page_directory[page_dir_i];
        if(!page_dir_ent) {
            page_dir_ent = page_directory[page_dir_i] = page_alloc_raw() | PD_PRESENT | PD_READ_WRITE;
        }
        uint32_t* page_tab = (uint32_t*)(page_dir_ent & PD_ADDR_MASK);
        page_tab[page_tab_i] = addr | PT_PRESENT | PT_READ_WRITE;
    }

    set_page_directory((phys_t)page_directory);
}

void
paging_init(multiboot_info_t* mb)
{
    extern int end_of_image;
    kernel_end = (size_t)&end_of_image;

    size_t pages_registered = register_available_memory(mb);
    printf("%d MiB available useful memory.\n", pages_registered * PAGE_SIZE / 1024 / 1024);

    identity_map_kernel();
}

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
page_alloc_raw()
{
    if(paging_enabled()) {
        panic("page_alloc_raw called while paging is enabled");
    }

    phys_t page = next_free_page;
    next_free_page = *(phys_t*)page;
    memset((void*)page, 0, PAGE_SIZE);
    return page;
}

void
page_free_raw(phys_t addr)
{
    if(paging_enabled()) {
        panic("page_free_raw called while paging is enabled");
    }

    // TODO same comment as above
    *(phys_t*)addr = next_free_page;
    next_free_page = addr;
}
