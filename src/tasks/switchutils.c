#include "../mem/include/paging.h"
#include "../utils/include/printf.h"
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