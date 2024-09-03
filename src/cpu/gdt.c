#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "../kernel/kernel.h"
#include "../utils/include/printf.h"
#include "include/gdt.h"
#include "../mem/include/pmm.h"

uint64_t create_gdt_entry(uint64_t base, uint64_t limit, uint64_t access, uint64_t flags) {
    uint64_t base1  = base & 0xFFFF;
    uint64_t base2  = (base >> 16) & 0xFF;
    uint64_t base3  = (base >> 24) & 0xFF;
    uint64_t limit1 = limit & 0xFFFF;
    uint64_t limit2 = limit >> 16;
    uint64_t entry  = 0;
    entry |= limit1;
    entry |= limit2 << 48;
    entry |= base1  << 16;
    entry |= base2  << 32;
    entry |= base3  << 56;
    entry |= access << 40;
    entry |= flags  << 52;
    return entry;
}

__attribute__((noinline))
void init_GDT() {
    printf(BYEL "[STATUS] " WHT "Initiating GDT... ");
    uint64_t *GDT = (uint64_t*) ((uint64_t)kmalloc(1) + ((uint64_t) kernel.hhdm));
    GDT[0] = create_gdt_entry(0, 0, 0, 0); // null
    GDT[1] = create_gdt_entry(0, 0, 0x9A, 0xA); // kernel code
    GDT[2] = create_gdt_entry(0, 0, 0x92, 0xC); // kernel data
    GDT[3] = create_gdt_entry(0, 0, 0xFA, 0xA); // user code
    GDT[4] = create_gdt_entry(0, 0, 0xF2, 0xC); // user data
    kernel.gdtr.size   = (sizeof(GDT[0]) * 5) - 1;
    kernel.gdtr.offset = (uint64_t) GDT;
    asm("lgdt (%0)" : : "r" (&kernel.gdtr));  
    asm volatile("push $0x08; \
              lea .reload_CS(%%rip), %%rax; \
              push %%rax; \
              retfq; \
              .reload_CS: \
              mov $0x10, %%ax; \
              mov %%ax, %%ds; \
              mov %%ax, %%es; \
              mov %%ax, %%fs; \
              mov %%ax, %%gs; \
              mov %%ax, %%ss" : : : "eax", "rax");
    printf(BGRN " Ok!\n" WHT);
}
