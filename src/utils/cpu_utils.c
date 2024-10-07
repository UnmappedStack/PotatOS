#include "include/cpu_utils.h"

void disable_interrupts() {
    asm("cli");
}

void enable_interrupts() {
    asm("sti");
}

void wait_for_interrupt() {
    asm("hlt");
}

void halt() {
    disable_interrupts();
    for (;;)
        wait_for_interrupt();
}

void cpuid(int code, uint32_t* a, uint32_t* d) {
    asm volatile( "cpuid" : "=a"(*a), "=d"(*d) : "0"(code) : "ebx", "ecx");
}