#pragma once

#include <stdint.h>

struct TSS {
    uint32_t rsvd0;
    uint64_t rsp0;
    uint32_t rsvd1[23]; // I *think* that should be 23 elements?
} __attribute__((packed));

void init_TSS();
