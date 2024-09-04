#include "../utils/include/printf.h"
#include "include/paging.h"
#include "../kernel/kernel.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

extern uint64_t p_kernel_start[];
extern uint64_t p_writeallowed_start[];
extern uint64_t p_kernel_end[];

uint64_t kernel_start       = (uint64_t) p_kernel_start;
uint64_t writeallowed_start = (uint64_t) p_writeallowed_start;
uint64_t kernel_end         = (uint64_t) p_kernel_end;

void map_sections(uint64_t pml4[]) {
    uint64_t num_memmap_entries                = kernel.memmap.entry_count;
    struct limine_memmap_entry *memmap_entries = *kernel.memmap.entries;
    for (size_t entry = 0; entry < num_memmap_entries; entry++) {
        uint64_t entry_type = memmap_entries[entry].type;
        if (entry_type == LIMINE_MEMMAP_USABLE ||
            entry_type == LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE ||
            entry_type == LIMINE_MEMMAP_FRAMEBUFFER ||
            entry_type == LIMINE_MEMMAP_KERNEL_AND_MODULES) {
            map_pages(pml4, memmap_entries[entry].base + kernel.hhdm, memmap_entries[entry].base, memmap_entries[entry].length / 4096, KERNEL_PFLAG_PRESENT | KERNEL_PFLAG_WRITE);
        }
    }
}

void map_kernel(uint64_t pml4[]) {
    uint64_t length_buffer = 0;
    uint64_t phys_buffer = 0;
    /* map from kernel_start to writeallowed_start with only the present flag */
    length_buffer = PAGE_ALIGN_UP(writeallowed_start - kernel_start);
    phys_buffer = kernel.kernel_addr.physical_base + (kernel_start - kernel.kernel_addr.virtual_base);
    map_pages(pml4, PAGE_ALIGN_DOWN(kernel_start), phys_buffer, length_buffer / 4096, KERNEL_PFLAG_PRESENT);
    /* map from writeallowed_start to kernel_end with `present` and `write` flags */
    length_buffer = PAGE_ALIGN_UP(kernel_end - writeallowed_start);
    phys_buffer = kernel.kernel_addr.physical_base + (writeallowed_start - kernel.kernel_addr.virtual_base);
    map_pages(pml4, PAGE_ALIGN_DOWN(writeallowed_start), phys_buffer, length_buffer / 4096, KERNEL_PFLAG_PRESENT | KERNEL_PFLAG_WRITE); 
    // map the kernel's stack
    alloc_pages(pml4, KERNEL_STACK_ADDR, KERNEL_STACK_PAGES, KERNEL_PFLAG_PRESENT | KERNEL_PFLAG_WRITE);
}

void map_all(uint64_t pml4[]) {
    map_kernel(pml4);
    map_sections(pml4);
}
