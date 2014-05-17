#include "console.h"
#include "paging.h"
#include "task.h"

void
sched_begin_multitasking_iret();

void
sched_begin_multitasking(task_t* initial_task)
{
    set_page_directory(initial_task->page_directory_phys);
    sched_begin_multitasking_iret();
}
