#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "include/idt.h"
#include "../mem/include/pmm.h"
#include "../kernel/kernel.h"
#include "../utils/include/printf.h"

void set_IDT_entry(uint32_t vector, void *isr, uint8_t flags, struct IDTEntry *IDT) {
    IDT[vector].offset1 = (uint64_t)isr;
    IDT[vector].offset2 = ((uint64_t)isr) >> 16;
    IDT[vector].offset3 = ((uint64_t)isr) >> 32;
    IDT[vector].flags   = flags;
    IDT[vector].segment_selector = 0x08;
}

__attribute__((interrupt))
void exceptionISR(void*) {
    printf("\nthere was an exception.\n");
    asm("cli; hlt");
}

void init_IDT() {
    printf(BYEL "[STATUS] " WHT "Initiating IDT... ");
    struct IDTEntry *IDT = (struct IDTEntry*) ((uint64_t)kmalloc(1) + ((uint64_t) kernel.hhdm));
    set_IDT_entry(0, &exceptionISR, 0x8F, IDT);
    kernel.idtr.size   = (sizeof(struct IDTEntry) * 256) - 1;
    kernel.idtr.offset = (uint64_t) IDT;
    asm("lidt %0" : : "m" (kernel.idtr));
    printf(BGRN " Ok!\n" WHT);
}
