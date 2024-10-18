#include "../kernel/kernel.h"
#include "../mem/include/paging.h"
#include "../utils/include/printf.h"
#include <stdint.h>
#include "include/tasklist.h"

uint64_t task_get_cr3(Task *task) {
    return (uint64_t) task->pml4_addr;
}

uint64_t task_get_entry_point(Task *task) {
    return (uint64_t) task->entry_point;
}

uint64_t task_get_flags(Task *task) {
    return task->flags;
}

uint64_t task_get_rsp(Task *task) {
    return task->current_rsp;
}

void task_disable_first_exec(Task *task) {
    task->flags &= ~TASK_FIRST_EXEC;
}

uint64_t task_get_kernel_stack(Task *task) {
    return task->kernel_rsp;
}

uint64_t task_get_argc(Task *task) {
    return task->argc;
}

uint64_t task_get_argv(Task *task) {
    return task->argv;
}

void task_set_rsp(Task *task, uint64_t rsp) {
    task->current_rsp = rsp;
}

bool check_task_switch_allowed() {
    return kernel.can_task_switch;
}
