#include "console.h"
#include "gdt.h"
#include "kernel_page.h"
#include "paging.h"
#include "panic.h"
#include "string.h"
#include "task.h"
#include "util.h"

static_assert(tss_t_is_0x68_bytes_long, sizeof(tss_t) == 0x68);
static_assert(task_t_is_544_bytes_long, sizeof(task_t) == 536);

static tss_t
tss;

static task_t
init_task;

task_t*
current_task;

void
task_init()
{
    gdt_set_tss(GDT_TSS, (uint32_t)&tss, sizeof(tss));
    gdt_reload();

    tss.ss0 = GDT_KERNEL_DATA;
    tss.esp0 = KERNEL_STACK_END;

    // pointer to the IO permission bitmap is beyond the end of the segment
    tss.iopb = sizeof(tss);

    __asm__ volatile("ltr ax" :: "a"((uint16_t)GDT_TSS | 3));

    // initialise task page directory
    uint32_t* current_page_directory = (uint32_t*)CURRENT_PAGE_DIRECTORY;
    uint32_t* task_page_directory = kernel_page_alloc_zeroed();

    if(!task_page_directory) {
        panic("could not allocate page for new task's page directory");
    }

    for(size_t i = 0; i < KERNEL_STACK_BEGIN / (4*1024*1024); i++) {
        task_page_directory[i] = current_page_directory[i];
    }

    init_task.page_directory = task_page_directory;
    init_task.page_directory_phys = virt_to_phys((virt_t)task_page_directory);
    init_task.page_directory[1023] = init_task.page_directory_phys | PE_PRESENT | PE_READ_WRITE;

    // initialize kernel stack page table for new task
    uint32_t* kernel_stack_page_table = kernel_page_alloc_zeroed();
    init_task.page_directory[KERNEL_STACK_BEGIN / (4*1024*1024)] = virt_to_phys((virt_t)kernel_stack_page_table) | PE_PRESENT | PE_READ_WRITE;

    init_task.kernel_stack = kernel_page_alloc_zeroed();
    kernel_stack_page_table[1023] = virt_to_phys((virt_t)init_task.kernel_stack) | PE_PRESENT | PE_READ_WRITE;

    // user stack
    uint32_t* user_stack_page_table = kernel_page_alloc_zeroed();
    init_task.page_directory[1022] = virt_to_phys((virt_t)user_stack_page_table) | PE_PRESENT | PE_READ_WRITE | PE_USER;
    user_stack_page_table[1023] = page_alloc() | PE_PRESENT | PE_READ_WRITE | PE_USER;

    current_task = &init_task;
}

void
task_init_load_text(const char* text, size_t size)
{
    set_page_directory(init_task.page_directory_phys);

    for(size_t i = 0; i < size; i += PAGE_SIZE) {
        phys_t page = page_alloc();
        page_map(USER_BEGIN + i, page, PE_PRESENT | PE_USER);
        size_t copy_size = size - i;
        if(copy_size > PAGE_SIZE) {
            copy_size = PAGE_SIZE;
        }
        memcpy((void*)(USER_BEGIN + i), (void*)(text + i), copy_size);
    }
}

void
task_destroy(task_t* task)
{
    kernel_page_free(task->kernel_stack);
    kernel_page_free((void*)(task->page_directory[KERNEL_STACK_BEGIN / (4*1024*1024)] & PE_ADDR_MASK));
    kernel_page_free(task->page_directory);
    memset(task, 0, sizeof(*task));
}
