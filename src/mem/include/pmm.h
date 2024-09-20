#pragma once

#include <stdint.h>

void init_PMM();

void* kmalloc(uint64_t num_pages);
