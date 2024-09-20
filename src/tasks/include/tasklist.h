#pragma once
#include "../../mem/include/vector.h"

#define TASK_PRESENT    0b001
#define TASK_FIRST_EXEC 0b010
#define TASK_RUNNING    0b100

typedef struct {
    uint64_t  pml4_addr;
    uint64_t  kernel_rsp;
    uint64_t  user_rsp;
    uintptr_t entry_point;
    uint8_t   flags;
} Task;

typedef struct {
    Vector   *list;
    uint64_t current_task;
} Tasklist;

void init_tasklist();
void create_task(uint64_t pml4_addr, uintptr_t entry_point, uintptr_t user_stack, uint8_t flags);
Task get_task(size_t pid);
Task* task_select();