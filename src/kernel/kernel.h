#pragma once

typedef struct {
    struct limine_memmap_response memmap;
    uint64_t hhdm;
} Kernel;

Kernel kernel;

