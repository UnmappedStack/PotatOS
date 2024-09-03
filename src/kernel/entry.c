#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "../cpu/include/idt.h"
#include "../cpu/include/gdt.h"
#include "../limine.h"
#include "kernel.h"
#include "../drivers/include/serial.h"
#include "../utils/include/printf.h"
#include "../mem/include/pmm.h"

/* Get info from Qemu */
static volatile struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0
};

static volatile struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0
};

void init_kernel_data() {
    kernel.memmap = *memmap_request.response;
    kernel.hhdm = (hhdm_request.response)->offset;
}

Kernel kernel = {};

void _start() {
    init_kernel_data();
    init_serial();
    printf("\nHigher Half Direct Mapping (HHDM): 0x%x\n", kernel.hhdm);
    init_PMM();
    init_GDT();
    init_IDT();
    write_serial("\nAll tasks halted, nothing left to do.\n");
    for(;;);
}
