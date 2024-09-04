#include "../kernel/kernel.h"
#include "../utils/include/printf.h"
#include <stddef.h>
#include <stdint.h>

void print_memory() {
    uint64_t num_memmap_entries                = kernel.memmap.entry_count;
    struct limine_memmap_entry *memmap_entries = *kernel.memmap.entries;
    for (size_t entry = 0; entry < num_memmap_entries; entry++) {
        printf("\nBase: 0x%x | Length: 0x%x | Type: %i", memmap_entries[entry].base, memmap_entries[entry].length, memmap_entries[entry].type);
    }
    printf("\n");
}
