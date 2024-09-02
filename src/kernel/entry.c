#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "../limine.h"
#include "kernel.h"
#include "../drivers/include/serial.h"
#include "../utils/include/printf.h"

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

void _start() {
    init_kernel_data();
    init_serial();
    write_serial("\nThis is PotatOS, the start of a new kernel to replace SpecOS and be far, far better!\nIt also has new lines :D\n");
    printf("Higher Half Direct Mapping (HHDM): 0x%x\n", kernel.hhdm);
    write_serial("\nAll tasks halted, nothing left to do.\n");
    for(;;);
}
