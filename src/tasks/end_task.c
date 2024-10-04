#include "include/end_task.h"
#include "../drivers/include/pit.h"
#include "../utils/include/printf.h"
#include "include/tasklist.h"
#include "../kernel/kernel.h"
#include "include/events.h"
#include "../utils/include/cpu_utils.h"

void try_exit_task(uint64_t exit_code) {
    disable_interrupts();
    Task *current_task = get_task(kernel.tasklist.current_task);
    if (current_task == NULL || !current_task->is_user) return;
    printf("\n");
    add_event(((Task*)current_task->parent)->event_queue, EVENT_TASK_EXITED, kernel.tasklist.current_task, exit_code);
    current_task->flags &= ~TASK_PRESENT;
    if ((kernel.tasklist.list)->length != 1) {
        enable_interrupts();
        unlock_pit();
        for (;;);
    } else {
        all_tasks_ended();
    }
}