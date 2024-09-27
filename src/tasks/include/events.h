#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "../../mem/include/vector.h"

// event types
#define EVENT_TASK_EXITED 1

typedef struct {
    bool do_handle;
    uint64_t event_id;
    uint64_t data0;
    uint64_t data1;
} Event;

typedef struct {
    Vector *queue;
} EventQueue;

EventQueue new_event_queue();
void add_event(EventQueue q, uint64_t event_type, uint64_t data0, uint64_t data1);
Event* get_event(EventQueue q);
void cleanup_event(Event *event);