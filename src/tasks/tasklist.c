#include "include/tasklist.h"
#include "include/events.h"
#include "../utils/include/string.h"
#include "../mem/include/paging.h"
#include "../mem/include/vector.h"
#include "../kernel/kernel.h"
#include "../mem/include/kheap.h"
#include "../utils/include/printf.h"
#include "../processors/include/spinlock.h"

Spinlock scheduler_lock;

void init_tasklist() {
    kstatusf("Initiating tasks...");
    kernel.tasklist.list = new_vector(sizeof(Task));
    Task *first_task = (Task*) malloc(sizeof(Task));
    char *current_dir = (char*) malloc(4);
    ku_memcpy(current_dir, "R:/", 4);
    *first_task = (Task) {
        .is_user     = false,
        .event_queue = new_event_queue(),
        .current_dir = current_dir,
        .pml4_addr   = kernel.cr3,
        .kernel_rsp  = KERNEL_STACK_PTR,
        .current_rsp = KERNEL_STACK_PTR,
        .entry_point = (uintptr_t) &all_tasks_ended,
        .parent      = (uintptr_t) first_task, // this task is it's own parent
        .flags       = 0
    };
    vector_push(kernel.tasklist.list, (uintptr_t) first_task);
    kernel.tasklist.current_tasks = new_vector(sizeof(uint64_t));
    for (size_t i = 0; i < kernel.smp_response->cpu_count; i++)
        vector_push(kernel.tasklist.current_tasks, 0);
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
    char *current_dir    = get_current_task()->current_dir;
    uint64_t dirname_len = ku_strlen(current_dir);
    new_task->current_dir = (char*) malloc(dirname_len) + 1;
    ku_memset(new_task->current_dir, 0, dirname_len);
    ku_memcpy(new_task->current_dir, current_dir, dirname_len + 1);
    vector_push(kernel.tasklist.list, (uintptr_t) new_task);
    return new_task;
}

Task* get_task(size_t pid) {
    return ((Task*) vector_at(kernel.tasklist.list, pid));
}

void task_remove(size_t pid) {
    vector_pop(kernel.tasklist.list, pid);
}

Spinlock get_current_task_lock;

Task* get_current_task() {
    spinlock_aquire(&get_current_task_lock);
    Task *to_return = get_task((uint64_t) vector_at(kernel.tasklist.current_tasks, get_current_processor()));
    spinlock_release(&get_current_task_lock);
    return to_return;
}

Task* task_select() {
    spinlock_aquire(&scheduler_lock);
    uint64_t current_core_num = get_current_processor();
    uint64_t current_task_num = (uint64_t) vector_at(kernel.tasklist.current_tasks, current_core_num);
    Task *current_task = get_task(current_task_num);
    current_task->flags &= ~((uint8_t) TASK_RUNNING);
    if ((kernel.tasklist.list)->length == 1) {
        vector_set(kernel.tasklist.current_tasks, current_core_num, 0);
        return get_task(0);
    }
    vector_set(kernel.tasklist.current_tasks, current_core_num, current_task_num + 1);
    current_task = vector_at(kernel.tasklist.current_tasks, current_core_num);
    if (current_task_num >= (kernel.tasklist.list)->length)
        vector_set(kernel.tasklist.current_tasks, current_core_num, 0);
    Task *new_task = get_current_task();
    if (new_task->flags & TASK_PRESENT && !(new_task->flags & TASK_RUNNING)) {
        new_task->flags |= TASK_RUNNING;
        spinlock_release(&scheduler_lock);
        return new_task;
    }  else {
        spinlock_release(&scheduler_lock);
        return task_select(); // switch to the next one again
    }
}
