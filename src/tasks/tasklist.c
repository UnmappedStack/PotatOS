#include "include/tasklist.h"
#include "../mem/include/paging.h"
#include "../mem/include/vector.h"
#include "../kernel/kernel.h"
#include "../mem/include/kheap.h"
#include "../utils/include/printf.h"

void init_tasklist() {
    kstatusf("Initiating tasks...");
    kernel.tasklist.list = new_vector(sizeof(Task));
    Task *first_task = (Task*) malloc(sizeof(Task));
    *first_task = (Task) {
        .is_user     = false,
        .pml4_addr   = kernel.cr3,
        .kernel_rsp  = KERNEL_STACK_PTR,
        .current_rsp = KERNEL_STACK_PTR,
        .entry_point = (uintptr_t) &all_tasks_ended,
        .flags       = 0
    };
    vector_push(kernel.tasklist.list, (uintptr_t) first_task);
    kernel.tasklist.current_task = 0;
    k_ok();
}

Task* create_task(uint64_t pml4_addr, uintptr_t entry_point, uintptr_t user_stack, uint8_t flags) {
    Task *new_task = (Task*) malloc(sizeof(Task));
    *new_task = (Task) {
        .is_user     = true,
        .pml4_addr   = pml4_addr,
        .kernel_rsp  = KERNEL_STACK_PTR,
        .entry_point = entry_point,
        .current_rsp = user_stack,
        .flags       = flags
    };
    vector_push(kernel.tasklist.list, (uintptr_t) new_task);
    return new_task;
}

Task* get_task(size_t pid) {
    return ((Task*) vector_at(kernel.tasklist.list, pid));
}

void task_remove(size_t pid) {
    vector_pop(kernel.tasklist.list, pid);
}

Task* task_select() {
    if ((kernel.tasklist.list)->length == 1) {
        kernel.tasklist.current_task = 0;
        return (Task*) vector_at(kernel.tasklist.list, 0);
    }
    else if (++kernel.tasklist.current_task >= (kernel.tasklist.list)->length)
        kernel.tasklist.current_task = 0;
    Task *new_task = (Task*) vector_at(kernel.tasklist.list, kernel.tasklist.current_task);
    if (new_task->flags & TASK_PRESENT)
        return new_task;
    else
        return task_select(); // switch to the next one again
}