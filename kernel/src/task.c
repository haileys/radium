#include "console.h"
#include "gdt.h"
#include "kernel_page.h"
#include "paging.h"
#include "panic.h"
#include "sched.h"
#include "string.h"
#include "task.h"
#include "util.h"

static_assert(tss_t_is_0x68_bytes_long, sizeof(tss_t) == 0x68);

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
        tasks[pid]->state = TASK_READY;
        tasks[pid]->pid = pid;
        tasks[pid]->wait_queue.live.head = NULL;
        tasks[pid]->wait_queue.live.tail = NULL;
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

    init_task->ppid = 0;

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

    sched_begin();
}

static void
copy_userland_pages(task_t* new_task)
{
    uint32_t* current_page_directory = (uint32_t*)CURRENT_PAGE_DIRECTORY;

    for(size_t dir_i = USER_BEGIN / (PAGE_SIZE * 1024); dir_i < 1023; dir_i++) {
        if(!current_page_directory[dir_i]) {
            continue;
        }

        uint32_t* current_page_table = (uint32_t*)(CURRENT_PAGE_TABLE_BASE + dir_i * PAGE_SIZE);

        uint32_t* new_page_table = kernel_page_alloc();
        new_task->page_directory[dir_i] =
            virt_to_phys((virt_t)new_page_table) | (current_page_directory[dir_i] & PE_FLAG_MASK);

        for(size_t tab_i = 0; tab_i < 1024; tab_i++) {
            uint32_t current_entry = current_page_table[tab_i];
            void* current_virt = (void*)((dir_i * PAGE_SIZE * 1024) + (tab_i * PAGE_SIZE));

            if(!(current_entry & PE_PRESENT)) {
                continue;
            }

            phys_t new_page = page_alloc();

            void* new_page_mapping = page_temp_map(new_page);
            memcpy(new_page_mapping, current_virt, PAGE_SIZE);
            page_temp_unmap();

            new_page_table[tab_i] = new_page | (current_entry & PE_FLAG_MASK);
        }
    }
}

task_t*
task_fork_inner()
{
    task_t* new_task = alloc_empty_task();
    create_skeleton_page_directory(new_task);

    memcpy(&new_task->fpu_state, &current_task->fpu_state, sizeof(new_task->fpu_state));
    memcpy(new_task->kernel_stack, current_task->kernel_stack, PAGE_SIZE);

    copy_userland_pages(new_task);

    new_task->ppid = current_task->pid;

    new_task->syscall_registers = current_task->syscall_registers;

    return new_task;
}

void
task_kill(task_t* task, uint8_t status)
{
    task->exit_status = status;

    task_t* parent = task_for_pid(task->ppid);

    // if this task has any children in the wait queue, just clean them up as
    // nothing is interested in them anymore.
    task_t* waitable_child = task->wait_queue.live.head;
    while(waitable_child) {
        task_t* next_waitable_child = waitable_child->wait_queue.dead.next;
        task_destroy(waitable_child);
        waitable_child = next_waitable_child;
    }

    // reparent children - TODO don't scan all processes...
    for(uint32_t i = 2; i < countof(tasks); i++) {
        task_t* child = task_for_pid(i);
        if(child && child->ppid == task->pid) {
            child->ppid = 1;
        }
    }

    // insert this task into the parent's wait queue
    if(parent->wait_queue.live.tail) {
        parent->wait_queue.live.tail->wait_queue.dead.next = task;
        parent->wait_queue.live.tail = task;
    } else {
        parent->wait_queue.live.head = task;
        parent->wait_queue.live.tail = task;
    }

    task->wait_queue.dead.next = NULL;

    // wake parent up if it's blocked in wait()
    if(parent->state == TASK_BLOCK_WAIT) {
        parent->state = TASK_READY;
    }

    // set state to EXITING so the scheduler never reschedules this task
    current_task->state = TASK_EXITING;
}

void
task_destroy(task_t* task)
{
    tasks[task->pid] = NULL;

    kernel_page_free(task->kernel_stack);

    // TODO - free pages allocated for the task that are referenced from its
    // page directory

    kernel_page_free(task->page_directory);
    kernel_page_free(task);
}

task_t*
sched_next()
{
    uint32_t current_pid = current_task->pid;

    for(size_t i = current_pid + 1; i < countof(tasks); i++) {
        task_t* task = task_for_pid(i);

        if(!task) {
            continue;
        }

        if(task->state == TASK_READY) {
            return task;
        }
    }

    for(size_t i = 1; i <= current_pid; i++) {
        task_t* task = task_for_pid(i);

        if(!task) {
            continue;
        }

        if(task->state == TASK_READY) {
            return task;
        }
    }

    panic("no tasks ready to schedule!");
}
