#ifndef PAGING_H
#define PAGING_H

#include "types.h"
#include "multiboot.h"

#define PAGE_SIZE 4096

#define PE_PRESENT    (1 << 0)
#define PE_READ_WRITE (1 << 1)
#define PE_USER       (1 << 2)
#define PE_FLAG_MASK  (PAGE_SIZE - 1)
#define PE_ADDR_MASK  (~PE_FLAG_MASK)

typedef uint32_t virt_t;
typedef uint32_t phys_t;

typedef enum {
    PAGE_KERNEL,
    PAGE_USER,
}
page_privilege_t;

typedef enum {
    PAGE_READ_ONLY,
    PAGE_READ_WRITE,
}
page_access_t;

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
page_map(virt_t virt_page, phys_t phys_page, int flags);

#endif
