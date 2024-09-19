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