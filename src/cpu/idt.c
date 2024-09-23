#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "include/idt.h"
#include "../mem/include/pmm.h"
#include "../drivers/include/pit.h"
#include "../kernel/kernel.h"
#include "../utils/include/printf.h"

void set_IDT_entry(uint32_t vector, void *isr, uint8_t flags, struct IDTEntry *IDT) {
    IDT[vector].offset1 = (uint64_t)isr;
    IDT[vector].offset2 = ((uint64_t)isr) >> 16;
    IDT[vector].offset3 = ((uint64_t)isr) >> 32;
    IDT[vector].flags   = flags;
    IDT[vector].segment_selector = 0x08;
}

extern void divideException();
extern void debugException();
extern void breakpointException();
extern void overflowException();
extern void boundRangeExceededException();
extern void invalidOpcodeException();
extern void deviceNotAvaliableException();
extern void doubleFaultException();
extern void coprocessorSegmentOverrunException();
extern void invalidTSSException();
extern void segmentNotPresentException();
extern void stackSegmentFaultException();
extern void generalProtectionFaultException();
extern void pageFaultException();
extern void floatingPointException();
extern void alignmentCheckException(); 
extern void machineCheckException();
extern void simdFloatingPointException();
extern void virtualisationException();

void init_IDT() {
    kstatusf("Initiating IDT... ");
    struct IDTEntry *IDT = (struct IDTEntry*) ((uint64_t)kmalloc(1) + ((uint64_t) kernel.hhdm)); 
    set_IDT_entry(0, &divideException, 0x8F, IDT);
    set_IDT_entry(1, &debugException, 0x8F, IDT);
    set_IDT_entry(3, &breakpointException, 0x8F, IDT);
    set_IDT_entry(4, &overflowException, 0x8F, IDT);
    set_IDT_entry(5, &boundRangeExceededException, 0x8F, IDT);
    set_IDT_entry(6, &invalidOpcodeException, 0x8F, IDT);
    set_IDT_entry(7, &deviceNotAvaliableException, 0x8F, IDT);
    set_IDT_entry(8, &doubleFaultException, 0x8F, IDT);
    set_IDT_entry(9, &coprocessorSegmentOverrunException, 0x8F, IDT);
    set_IDT_entry(10, &invalidTSSException, 0x8F, IDT);
    set_IDT_entry(11, &segmentNotPresentException, 0x8F, IDT);
    set_IDT_entry(12, &stackSegmentFaultException, 0x8F, IDT);
    set_IDT_entry(13, &generalProtectionFaultException, 0x8F, IDT);
    set_IDT_entry(14, &pageFaultException, 0x8F, IDT);
    set_IDT_entry(16, &floatingPointException, 0x8F, IDT);
    set_IDT_entry(17, &alignmentCheckException, 0x8F, IDT);
    set_IDT_entry(18, &machineCheckException, 0x8F, IDT);
    set_IDT_entry(19, &simdFloatingPointException, 0x8F, IDT);
    set_IDT_entry(20, &virtualisationException, 0x8F, IDT);
    kernel.idtr.size   = (sizeof(struct IDTEntry) * 256) - 1;
    kernel.idtr.offset = (uint64_t) IDT;
    asm("lidt %0" : : "m" (kernel.idtr));
    printf(" Ok!\n");
}
