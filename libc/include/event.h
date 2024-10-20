#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <syscall.h>

typedef struct {
    bool do_handle;
    uint64_t event_id;
    uint64_t data0;
    uint64_t data1;
} Event;

// event types
#define EVENT_TASK_EXITED 1

int poll(Event *buffer);
int peek(Event *buffer);
