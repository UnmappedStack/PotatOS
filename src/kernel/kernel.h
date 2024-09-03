#pragma once

#include <stdint.h>
#include "../limine.h"

typedef struct {
    struct   limine_memmap_response memmap;
    uint64_t hhdm;
    long     last_freed_page;
    long     last_freed_section;
    uint64_t last_freed_num_pages;
} Kernel;

extern Kernel kernel;
