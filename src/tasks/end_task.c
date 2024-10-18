#include "include/end_task.h"
#include "../drivers/include/pit.h"
#include "../utils/include/printf.h"
#include "include/tasklist.h"
#include "../mem/include/vector.h"
#include "../drivers/include/apic.h"
#include "../kernel/kernel.h"
#include "include/events.h"
#include "../utils/include/cpu_utils.h"

void syscall_exit(uint64_t exit_code) {
    disable_interrupts();
    uint64_t current_core = get_current_processor();
    uint64_t current_task_num = (uint64_t) vector_at(kernel.tasklist.current_tasks, current_core);
    Task *current_task = get_current_task();
    if (current_task == NULL || !current_task->is_user) return;
    add_event(((Task*)current_task->parent)->event_queue, EVENT_TASK_EXITED, current_task_num, exit_code);
    current_task->flags &= ~TASK_PRESENT;
    if ((kernel.tasklist.list)->length != 1) {
        enable_interrupts();
        unlock_pit();
        for (;;);
    } else {
        all_tasks_ended();
    }
}
