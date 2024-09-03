#pragma once

#include <stdint.h>

void init_PMM();

void* kmalloc(uint32_t num_pages);
