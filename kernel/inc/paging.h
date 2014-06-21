#ifndef PAGING_H
#define PAGING_H

#include "types.h"
#include "multiboot.h"

#define PAGE_SIZE 4096

#define PE_FLAG_MASK  (PAGE_SIZE - 1)
#define PE_ADDR_MASK  (~PE_FLAG_MASK)

#define KERNEL_STACK_BEGIN       0x0fc00000ul
#define KERNEL_STACK_END         0x0ffffffcul
#define USER_BEGIN               0x10000000ul
#define USER_STACK_END           0xffc00000ul
#define CURRENT_PAGE_DIRECTORY   0xfffff000ul
#define CURRENT_PAGE_TABLE_BASE  0xffc00000ul

typedef enum {
    PE_PRESENT    = 1 << 0,
    PE_READ_WRITE = 1 << 1,
    PE_USER       = 1 << 2,
}
page_flags_t;

void
paging_set_allocatable_start(phys_t addr);

void
paging_init(struct multiboot_info* mb);

void
set_page_directory(phys_t page_directory);

phys_t
page_alloc();

void
page_free(phys_t addr);

void
page_map(virt_t virt_page, phys_t phys_page, page_flags_t flags);

void
page_unmap(virt_t virt_page);

phys_t
virt_to_phys(virt_t virt);

void*
page_temp_map(phys_t phys_page);

void
page_temp_unmap();

bool
page_is_user_mapped(virt_t virt);

#endif
