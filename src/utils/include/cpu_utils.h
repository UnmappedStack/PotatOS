#pragma once
#include <stdint.h>

void disable_interrupts();
void enable_interrupts();
void wait_for_interrupt();
void halt();
void cpuid(int code, uint32_t* a, uint32_t* d);