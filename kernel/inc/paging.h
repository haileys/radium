#ifndef PAGING_H
#define PAGING_H

#include <types.h>

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

bool
paging_enabled();

void
set_page_directory(phys_t page_directory);

void
paging_init(struct multiboot_info* mb);

phys_t
page_alloc_raw();

void
page_free_raw(phys_t addr);

#endif
