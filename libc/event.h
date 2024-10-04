#pragma once
#include <stdio.h>

typedef struct {
    bool do_handle;
    uint64_t event_id;
    uint64_t data0;
    uint64_t data1;
} Event;

void poll(Event *buffer);

#ifndef POLL_IMPL
void poll(Event *buffer) {
    fputs("", stdout); // it seems to only work when I have an fputs here for some reason, so I'm taking the broken solution for now lmao
    asm volatile(
        "int $0x80"
        : : "D" ((uint64_t) buffer), "a" (6)
    );
}
#endif

void peek(Event *buffer);

#ifndef PEEK_IMPL
void peek(Event *buffer) {
    fputs("", stdout); // it seems to only work when I have an fputs here for some reason, so I'm taking the broken solution for now lmao
    asm volatile(
        "int $0x80"
        : : "D" ((uint64_t) buffer), "a" (7)
    );
}
#endif