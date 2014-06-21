#include "console.h"
#include "gdt.h"
#include "idt.h"
#include "kernel_page.h"
#include "paging.h"
#include "panic.h"
#include "sched.h"
#include "string.h"
#include "task.h"
#include "util.h"

static_assert(tss_t_is_0x68_bytes_long, sizeof(tss_t) == 0x68);
static_assert(task_t_is_540_bytes_long, sizeof(task_t) == 540);

static tss_t
tss;

task_t*
current_task;

static task_t*
tasks[1024];

task_t*
task_for_pid(uint32_t pid)
{
    if(pid > countof(tasks)) {
        return NULL;
    }

    return tasks[pid];
}

static task_t*
alloc_empty_task()
{
    for(uint32_t pid = 1; pid < countof(tasks); pid++) {
        if(tasks[pid]) {
            continue;
        }

        tasks[pid] = kernel_page_alloc_zeroed();
        tasks[pid]->pid = pid;
        return tasks[pid];
    }

    return NULL;
}

static void
create_skeleton_page_directory(task_t* task)
{
    // initialise task page directory
    uint32_t* current_page_directory = (uint32_t*)CURRENT_PAGE_DIRECTORY;
    uint32_t* task_page_directory = kernel_page_alloc_zeroed();

    if(!task_page_directory) {
        panic("could not allocate page for new task's page directory");
    }

    for(size_t i = 0; i < KERNEL_STACK_BEGIN / (4*1024*1024); i++) {
        task_page_directory[i] = current_page_directory[i];
    }

    task->page_directory = task_page_directory;
    task->page_directory_phys = virt_to_phys((virt_t)task_page_directory);
    task->page_directory[1023] = task->page_directory_phys | PE_PRESENT | PE_READ_WRITE;

    uint32_t* kernel_stack_page_table = kernel_page_alloc_zeroed();
    task->page_directory[KERNEL_STACK_BEGIN / (4*1024*1024)] = virt_to_phys((virt_t)kernel_stack_page_table) | PE_PRESENT | PE_READ_WRITE;

    task->kernel_stack = kernel_page_alloc_zeroed();
    kernel_stack_page_table[1023] = virt_to_phys((virt_t)task->kernel_stack) | PE_PRESENT | PE_READ_WRITE;
}

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

    task_t* init_task = alloc_empty_task();
    create_skeleton_page_directory(init_task);

    // user stack
    uint32_t* user_stack_page_table = kernel_page_alloc_zeroed();
    init_task->page_directory[1022] = virt_to_phys((virt_t)user_stack_page_table) | PE_PRESENT | PE_READ_WRITE | PE_USER;
    user_stack_page_table[1023] = page_alloc() | PE_PRESENT | PE_READ_WRITE | PE_USER;

    current_task = init_task;
}

void
task_boot_init(const char* text, size_t size)
{
    task_t* init_task = task_for_pid(1);

    set_page_directory(init_task->page_directory_phys);

    for(size_t i = 0; i < size; i += PAGE_SIZE) {
        phys_t page = page_alloc();
        page_map(USER_BEGIN + i, page, PE_PRESENT | PE_USER);
        size_t copy_size = size - i;
        if(copy_size > PAGE_SIZE) {
            copy_size = PAGE_SIZE;
        }
        memcpy((void*)(USER_BEGIN + i), (void*)(text + i), copy_size);
    }

    critical_reset();
    sched_begin();
}

void
task_destroy(task_t* task)
{
    kernel_page_free(task->kernel_stack);
    kernel_page_free((void*)(task->page_directory[KERNEL_STACK_BEGIN / (4*1024*1024)] & PE_ADDR_MASK));
    kernel_page_free(task->page_directory);
    memset(task, 0, sizeof(*task));
}
