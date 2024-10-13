#pragma once
#include <stdio.h>

typedef struct {
    bool do_handle;
    uint64_t event_id;
    uint64_t data0;
    uint64_t data1;
} Event;

// event types
#define EVENT_TASK_EXITED 1

void poll(Event *buffer);

#ifndef POLL_IMPL
void poll(Event *buffer) {
    asm volatile(
        "int $0x80"
        : : "D" ((uint64_t) buffer), "a" (6)
    );
}
#endif

void peek(Event *buffer);

#ifndef PEEK_IMPL
void peek(Event *buffer) {
    asm volatile(
        "int $0x80"
        : : "D" ((uint64_t) buffer), "a" (7)
    );
}
#endif