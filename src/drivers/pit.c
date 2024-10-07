#include "include/pit.h"
#include "../utils/include/printf.h"
#include "../utils/include/io.h"
#include "../cpu/include/idt.h"
#include "../drivers/include/apic.h"
#include "../kernel/kernel.h"
#include "../utils/include/cpu_utils.h"

#define HERTZ_DIVIDER 11900

extern void pit_isr();

void init_PIT() {
    kstatusf("Initialising PIT clock...\n");
    outb(0x43, 0b110100); // set mode to rate generator, channel 0, lobyte/hibyte, binary mode
    outb(0x40, (HERTZ_DIVIDER) & 0xFF);
    outb(0x40, (HERTZ_DIVIDER >> 8) & 0xFF);
    set_IDT_entry(32, &pit_isr, 0x8F, (struct IDTEntry*) kernel.idtr.offset);
    map_ioapic(32, 2, 0, POLARITY_HIGH, TRIGGER_EDGE);
    lock_pit();
    kstatusf("Successfully initiated PIT.\n");
}

void unlock_pit() {
    unmask_ioapic(2, 0);
}

void lock_pit() {
    mask_ioapic(2, 0);
}

