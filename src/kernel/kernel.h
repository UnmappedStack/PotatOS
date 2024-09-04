#pragma once

#include <stdint.h>
#include "../limine.h"
#include "../cpu/include/gdt.h"
#include "../cpu/include/tss.h"
#include "../cpu/include/idt.h"

typedef struct {
    struct      limine_memmap_response memmap;
    struct      limine_kernel_address_response kernel_addr;
    uint64_t    hhdm;
    long        last_freed_page;
    long        last_freed_section;
    uint64_t    last_freed_num_pages;
    struct GDTR gdtr;
    struct IDTR idtr;
    struct TSS  tss;
    uint64_t    cr3;
} Kernel;

extern Kernel kernel;
