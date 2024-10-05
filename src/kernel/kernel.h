#pragma once

#include <stdint.h>
#include "../drivers/include/acpi.h"
#include "../drivers/include/renderfont.h"
#include "../limine.h"
#include "../mem/include/vector.h"
#include "../cpu/include/gdt.h"
#include "../cpu/include/tss.h"
#include "../cpu/include/idt.h"
#include "../fs/include/vfs.h"
#include "../tasks/include/tasklist.h"

typedef struct {
    struct      limine_memmap_response memmap;
    struct      limine_kernel_address_response kernel_addr;
    struct      limine_framebuffer **framebuffers;
    struct      limine_file *initial_ramdisk;
    struct      limine_smp_response *smp_response;
    RSDP        *rsdp_table;
    RSDT        *rsdt;
    volatile uint64_t framebuffer_size;
    psf1Header  *font_info;
    uint8_t     *font_data;
    uint32_t    fg_colour;
    uint32_t    bg_colour;
    bool        font_avaliable;
    uint64_t    ch_X;
    uint64_t    ch_Y;
    uint32_t    *back_buffer;
    bool        in_exception_handler;
    uint64_t    hhdm;
    long        last_freed_page;
    long        last_freed_section;
    uint64_t    last_freed_num_pages;
    uintptr_t   kheap_start;
    Tasklist    tasklist;
    struct GDTR gdtr;
    struct IDTR idtr;
    struct TSS  tss;
    uint64_t    cr3;
    Drive drives[26];
} Kernel;

extern Kernel kernel;

void all_tasks_ended();