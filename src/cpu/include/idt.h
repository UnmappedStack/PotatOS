#pragma once

#include <stdint.h>

struct IDTEntry {
    uint16_t offset1;
    uint16_t segment_selector;
    uint8_t rsvd;
    uint8_t flags;
    uint16_t offset2;
    uint32_t offset3;
    uint32_t reserved;
} __attribute__((packed));

struct IDTR {
    uint16_t size;
    uint64_t offset;
} __attribute__((packed));

void init_IDT();
void set_IDT_entry(uint32_t vector, void *isr, uint8_t flags, struct IDTEntry *IDT);