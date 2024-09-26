#include "include/end_task.h"
#include "../drivers/include/pit.h"
#include "../utils/include/printf.h"
#include "include/tasklist.h"
#include "../kernel/kernel.h"
#include "../utils/include/cpu_utils.h"

void try_exit_task(uint64_t exit_code) {
    Task *current_task = get_task(kernel.tasklist.current_task);
    if (current_task == NULL || !current_task->is_user) return;
    printf("\n");
    kstatusf("Exited process of PID %i with status code %i.\n", kernel.tasklist.current_task, exit_code);
    task_remove(kernel.tasklist.current_task);
    if ((kernel.tasklist.list)->length != 1) {
        unlock_pit();
        enable_interrupts();
        for (;;);
    } else {
        all_tasks_ended();
    }
}