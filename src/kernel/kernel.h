#pragma once

#include <stdint.h>
#include "../limine.h"
#include "../mem/include/vector.h"
#include "../cpu/include/gdt.h"
#include "../cpu/include/tss.h"
#include "../cpu/include/idt.h"
#include "../fs/include/vfs.h"

typedef struct {
    struct      limine_memmap_response memmap;
    struct      limine_kernel_address_response kernel_addr;
    struct      limine_framebuffer **framebuffers;
    struct      limine_file *initial_ramdisk;
    struct      limine_smp_response *smp_response;
    bool        in_exception_handler;
    uint64_t    hhdm;
    long        last_freed_page;
    long        last_freed_section;
    uint64_t    last_freed_num_pages;
    uintptr_t   kheap_start;
    Vector      *tasklist;
    struct GDTR gdtr;
    struct IDTR idtr;
    struct TSS  tss;
    uint64_t    cr3;
    Drive drives[26];
} Kernel;

extern Kernel kernel;

void _start(); // This is just needed so that the task list can get the address of the kernel entry point.