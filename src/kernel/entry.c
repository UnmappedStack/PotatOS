#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "../mem/include/vector.h"
#include "../fs/include/ustar.h"
#include "../drivers/include/framebuffer.h"
#include "../fs/include/vfs.h"
#include "../fs/include/tempfs.h"
#include "../mem/include/kheap.h"
#include "../mem/include/print_mem.h"
#include "../mem/include/paging.h"
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

static volatile struct limine_kernel_address_request kernel_address_request = {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST,
    .revision = 2
};

static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

static volatile struct limine_internal_module initrd = {
    .path = "initrd",
    .flags = LIMINE_INTERNAL_MODULE_REQUIRED
};

struct limine_internal_module *module_list = &initrd;

__attribute__((used, section(".requests")))
static volatile struct limine_module_request initrd_request = {
    .id = LIMINE_MODULE_REQUEST,
    .revision = 0,
    .internal_modules = &module_list,
    .internal_module_count = 1
};

void init_kernel_data() {
    kernel.memmap          = *memmap_request.response;
    kernel.hhdm            = (hhdm_request.response)->offset;
    kernel.kernel_addr     = *(kernel_address_request.response);
    kernel.framebuffers    = (framebuffer_request.response)->framebuffers;
    kernel.initial_ramdisk = *((initrd_request.response)->modules);
}

Kernel kernel = {0};

void show_boot_info() {
    kdebugf("Higher Half Direct Mapping (HHDM): 0x%x\n", kernel.hhdm);
    kdebugf("Memory map recieved from bootloader:");
    print_memory();
    printf("\n");
}

void _start() {
    init_kernel_data();
    fill_screen(0x0000FF);
    init_serial();
    show_boot_info();
    init_PMM();
    init_kheap();
    init_GDT();
    init_TSS();
    init_IDT();
    init_paging();
    switch_page_structures();
    init_vfs();
    setup_initrd();
    kstatusf("All tasks halted, nothing left to do.\n\n");
    fill_screen(0x00FF00);
    for(;;);
}
