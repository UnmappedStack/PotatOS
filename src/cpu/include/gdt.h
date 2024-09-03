#pragma once

struct GDTR {
    uint16_t size;
    uint64_t offset;
} __attribute__((packed));

void init_GDT();
