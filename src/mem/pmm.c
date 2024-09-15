/* This is possibly a messy file, and I'm sure there's a number of ways it could be improved.
 * Please open an issue if you have any specific ideas.
 */

#include "../utils/include/string.h"
#include "../kernel/kernel.h"
#include "include/pmm.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "../utils/include/printf.h"

#define LIMINE_MEMMAP_USABLE                 0
#define LIMINE_MEMMAP_RESERVED               1
#define LIMINE_MEMMAP_ACPI_RECLAIMABLE       2
#define LIMINE_MEMMAP_ACPI_NVS               3
#define LIMINE_MEMMAP_BAD_MEMORY             4
#define LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE 5
#define LIMINE_MEMMAP_KERNEL_AND_MODULES     6
#define LIMINE_MEMMAP_FRAMEBUFFER            7

uint64_t get_bitmap_reserved(struct limine_memmap_entry memmap_entry) {
    uint64_t bitmap_reserved = 0;
    uint32_t n = 1;
    while (true) {
        if (n * 4096 * 8 > (memmap_entry.length / 4096) - n) {
            bitmap_reserved = n * 4096;
            break;
        }
    }
    return bitmap_reserved;
}

void init_single_bitmap(struct limine_memmap_entry memmap_entry) {
    if (memmap_entry.type != LIMINE_MEMMAP_USABLE) return; 
    ku_memset((uint8_t*) memmap_entry.base + kernel.hhdm, 0, memmap_entry.length);
}

void init_PMM() {
    kstatusf("Initiating physical memory allocator...");
    struct limine_memmap_entry *memmap_entries = *kernel.memmap.entries;
    uint64_t num_memmap_entries = kernel.memmap.entry_count;
    for (size_t entry = 0; entry < num_memmap_entries; entry++)
        init_single_bitmap(memmap_entries[entry]);
    kernel.last_freed_section = -1;
    kernel.last_freed_page    = -1;
    printf(BGRN " Ok!\n" WHT);
}

void allocate_page(uint64_t section_index, uint64_t page_frame_number) {
    uint8_t  *bitmap_start = (uint8_t*) (*kernel.memmap.entries)[section_index].base + kernel.hhdm;
    uint64_t byte          = page_frame_number / 8;
    uint64_t bit           = page_frame_number % 8;
    uint8_t  or_value      = (bit) ? (1 << bit) : 1;
    uint8_t  *current_byte = (uint8_t*) (((uint64_t) bitmap_start) + (uint64_t)(byte));
    *current_byte = *current_byte | or_value;
}

void allocate_pages(uint64_t section_index, uint64_t page_frame_number, size_t num_pages) {
    for (size_t i = 0; i < num_pages; i++)
        allocate_page(section_index, page_frame_number + i);
}

bool check_pages_avaliable(uint64_t section_index, uint64_t page_frame_number, uint64_t num_pages, uint64_t bitmap_reserved) {
    // check if `num_pages` pages are avaliable
    uint64_t pages_in_section = bitmap_reserved * 8;
    uint64_t top_page = page_frame_number + num_pages;
    if (top_page > pages_in_section) return false;
    uint8_t *bitmap_start = (uint8_t*) (*kernel.memmap.entries)[section_index].base + kernel.hhdm;
    for (; page_frame_number < top_page; page_frame_number++) {
        // check if the single page is avaliable. If not, break.
        uint64_t byte = page_frame_number / 8;
        uint64_t bit  = page_frame_number % 8;
        if ((bitmap_start[byte] >> bit) & 1) return false;
    }
    // No pages were marked as used.
    return true;
}

void* kmalloc(uint32_t num_pages) {
    if (num_pages < 1) {
        kfailf("Cannot allocate less than 1 page! Halting device.\n");
        asm("cli; hlt");
    }
    struct limine_memmap_entry *memmap_entries = *kernel.memmap.entries;
    if (kernel.last_freed_page != -1 && num_pages < kernel.last_freed_num_pages) {
        allocate_pages(kernel.last_freed_section, kernel.last_freed_page, num_pages);
        uint64_t bitmap_reserved = get_bitmap_reserved(memmap_entries[kernel.last_freed_section]);
        return (void*) (memmap_entries[kernel.last_freed_section].base + (kernel.last_freed_page * 4096) + bitmap_reserved);
    }
    for (size_t entry = 0; entry < kernel.memmap.entry_count; entry++) {
        // look through this entry for avaliable pages
        if (memmap_entries[entry].type != LIMINE_MEMMAP_USABLE) continue;
        uint64_t bitmap_reserved = get_bitmap_reserved(memmap_entries[entry]);
        uint64_t max_bitmap_bytes = ((memmap_entries[entry].length - bitmap_reserved) / 4096) / 8;
        for (size_t bitmap_byte = 0; bitmap_byte < max_bitmap_bytes; bitmap_byte++) {
            for (uint8_t bitmap_bit = 0; bitmap_bit < 8; bitmap_bit++) {
                if (check_pages_avaliable(entry, bitmap_bit + (bitmap_byte * 8), num_pages, bitmap_reserved)) {
                    allocate_pages(entry, bitmap_bit + (bitmap_byte * 8), num_pages);
                    uintptr_t addr = (memmap_entries[entry].base + ((bitmap_bit + (bitmap_byte * 8)) * 4096) + bitmap_reserved);
                    ku_memset((uint8_t*) addr, 0, num_pages * 4096);
                    return (void*) addr;
                }
            }
        }
    }
    // no avaliable space! panic.
    kfailf("No more avaliable space in RAM to allocate! Trying to allocate %i pages. Halting device.\n",
           num_pages);
    asm("cli; hlt");
    return 0; // random val to please the compiler
}
















