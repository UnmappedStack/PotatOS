#include "include/pit.h"
#include "../utils/include/printf.h"
#include "../utils/include/io.h"
#include "../cpu/include/idt.h"
#include "../drivers/include/irq.h"
#include "../kernel/kernel.h"

#define HERTZ_DIVIDER 1190 // this would be about 1 ms

__attribute__((interrupt))
void pit_test_isr(void*) {
    printf("Got interrupt from PIT\n");
    end_of_interrupt();
}

void init_PIT() {
    kstatusf("Initialising PIT clock...");
    outb(0x43, 0b110100); // set mode to rate generator, channel 0, lobyte/hibyte, binary mode
    outb(0x40, (HERTZ_DIVIDER) & 0xFF);
    outb(0x40, (HERTZ_DIVIDER >> 8) & 0xFF);
    set_IDT_entry(32, &pit_test_isr, 0x8E, (struct IDTEntry*) kernel.idtr.offset);
    lock_pit();
    printf(BGRN " Ok!\n" WHT);
}

void unlock_pit() {
    unmask_irq(0);
}

void lock_pit() {
    mask_irq(0);
}

