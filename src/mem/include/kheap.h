#pragma once

#include <stdint.h>

void init_kheap();

void* malloc(uint64_t size);

void free(void* addr);
