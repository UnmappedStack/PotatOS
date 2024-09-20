#include "include/tasklist.h"
#include "../mem/include/paging.h"
#include "../mem/include/vector.h"
#include "../kernel/kernel.h"
#include "../mem/include/kheap.h"
#include "../utils/include/printf.h"

void init_tasklist() {
    kstatusf("Initiating task list...");
    kernel.tasklist.list = new_vector(sizeof(Task));
    Task *first_task = (Task*) malloc(sizeof(Task));
    *first_task = (Task) {
        .pml4_addr   = kernel.cr3,
        .kernel_rsp  = KERNEL_STACK_PTR,
        .entry_point = (uintptr_t) &_start,
        .flags       = TASK_PRESENT | TASK_RUNNING
    };
    vector_push(kernel.tasklist.list, (uintptr_t) first_task);
    kernel.tasklist.current_task = 0;
    printf(BGRN " Ok!\n" WHT);
}

void create_task(uint64_t pml4_addr, uintptr_t entry_point, uintptr_t user_stack, uint8_t flags) {
    Task *new_task = (Task*) malloc(sizeof(Task));
    *new_task = (Task) {
        .pml4_addr   = pml4_addr,
        .kernel_rsp  = KERNEL_STACK_PTR,
        .entry_point = entry_point,
        .user_rsp    = user_stack,
        .flags       = flags
    };
    vector_push(kernel.tasklist.list, (uintptr_t) new_task);
}

Task get_task(size_t pid) {
    return *((Task*) vector_at(kernel.tasklist.list, pid));
}

Task* task_select() {
    if ((kernel.tasklist.list)->length >= kernel.tasklist.current_task)
        kernel.tasklist.current_task = 0;
    else
        kernel.tasklist.current_task++;
    return (Task*) vector_at(kernel.tasklist.list, kernel.tasklist.current_task);
}