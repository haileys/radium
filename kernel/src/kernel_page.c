#include "idt.h"
#include "kernel_page.h"
#include "paging.h"
#include "panic.h"

typedef union kernel_page {
    union kernel_page* next;
    char mem[PAGE_SIZE];
}
kernel_page_t;

static kernel_page_t*
allocated_to;

static kernel_page_t*
end;

static kernel_page_t*
next_free;

void
kernel_page_init(virt_t begin_, virt_t end_)
{
    allocated_to = (void*)begin_;
    end = (void*)end_;
}

void*
kernel_page_alloc()
{
    interrupts_disable();

    if(next_free) {
        kernel_page_t* page = next_free;
        next_free = next_free->next;
        interrupts_enable();
        return page;
    }

    interrupts_enable();

    if(allocated_to >= end) {
        return NULL;
    }

    page_map((virt_t)allocated_to, page_alloc(), PE_PRESENT | PE_READ_WRITE);
    return allocated_to++;
}

void
kernel_page_free(void* ptr)
{
    kernel_page_t* page = ptr;

    interrupts_disable();
    page->next = next_free;
    next_free = page;
    interrupts_enable();
}
