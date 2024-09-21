#include "../mem/include/paging.h"
#include "../utils/include/printf.h"
#include "include/switch.h"
#include "include/tasklist.h"

void task_switch_page_tree(Task *task) {
    KERNEL_SWITCH_PAGE_TREE(task->pml4_addr);
}

uint64_t task_get_entry_point(Task *task) {
    return (uint64_t) task->entry_point;
}

uint64_t task_get_flags(Task *task) {
    return task->flags;
}