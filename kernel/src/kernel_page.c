#include "kernel_page.h"
#include "paging.h"
#include "panic.h"
#include "string.h"

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
    if(next_free) {
        kernel_page_t* page = next_free;
        next_free = next_free->next;
        return page;
    }

    if(allocated_to >= end) {
        return NULL;
    }

    page_map((virt_t)allocated_to, page_alloc(), PE_PRESENT | PE_READ_WRITE);
    void* retn = allocated_to++;

    return retn;
}

void*
kernel_page_alloc_zeroed()
{
    void* page = kernel_page_alloc();
    memset32(page, 0, 1024);
    return page;
}

void
kernel_page_free(void* ptr)
{
    kernel_page_t* page = ptr;

    page->next = next_free;
    next_free = page;
}
