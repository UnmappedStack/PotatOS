#pragma once

typedef struct {
    bool do_handle;
    uint64_t event_id;
    uint64_t data0;
    uint64_t data1;
} Event;

void get_event(Event *buffer);

// syscall 2
// rdi = event buffer

#ifndef GET_EVENT_IMPL
void get_event(Event *buffer) {
    printf(""); // it seems to only work when I have a printf here for some reason, so I'm taking the broken solution for now lmao
    asm volatile(
        "movq %0, %%rdi\n"
        "movq $2, %%rax\n"
        "xor %%rsi, %%rsi\n"
        "xor %%rdx, %%rdx\n"
        "int $0x80"
        : : "r" ((uint64_t) buffer)
        : "%rdi", "%rax", "%rdi", "%rdx" 
    );
}
#endif