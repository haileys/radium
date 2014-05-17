#ifndef KALLOC_H
#define KALLOC_H

#include "types.h"

void
kernel_page_init(virt_t begin, virt_t end);

void*
kernel_page_alloc();

void*
kernel_page_alloc_zeroed();

void
kernel_page_free(void*);

#endif
