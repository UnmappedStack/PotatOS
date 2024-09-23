#include "include/irq.h"
#include "../utils/include/printf.h"
#include "../utils/include/io.h"


void init_irq() {
    kstatusf("Initiating IRQs...");
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0xFF);
    outb(0xA1, 0xFF);
    printf(" Ok!\n");
}

void unmask_irq(int IRQ) {
    if (IRQ < 8)
        outb(0x21, ~(1 << (IRQ % 8)));
    else
        outb(0xA1, ~(1 << (IRQ % 8)));
}

void mask_irq(int IRQ) {
    if (IRQ < 8)
        outb(0x21, (1 << (IRQ % 8)));
    else
        outb(0xA1, (1 << (IRQ % 8)));
}

void end_of_interrupt() {
    outb(0x20, 0x20);
}