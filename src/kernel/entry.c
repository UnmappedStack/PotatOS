#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

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

void init_kernel_data() {
    kernel.memmap      = *memmap_request.response;
    kernel.hhdm        = (hhdm_request.response)->offset;
    kernel.kernel_addr = *(kernel_address_request.response);
}

Kernel kernel = {0};


#define KERNEL_SWITCH_STACK() \
    __asm__ volatile (\
       "movq %0, %%rsp\n"\
       "movq $0, %%rbp\n"\
       "push $0"\
       :\
       :  "r" (KERNEL_STACK_PTR)\
    )

void show_boot_info() {
    kdebugf("Higher Half Direct Mapping (HHDM): 0x%x\n", kernel.hhdm);
    kdebugf("Memory map recieved from bootloader:");
    print_memory();
    printf("\n");
}

static void switch_page_structures() {
    kstatusf("Switching CR3 & kernel stack...");
    KERNEL_SWITCH_PAGE_TREE(kernel.cr3);
    KERNEL_SWITCH_STACK();
    printf(BGRN " Ok!\n" WHT);
}

// TODO: move to a seperate file
void setup_initrd() {
    kstatusf("Creating new TempFS...");
    Inode *new_tempfs = tempfs_new();
    printf(BGRN " Ok!\n" WHT);
    kstatusf("Mounting TempFS onto VFS drive `R:/`...");
    mount('R', FS_TEMPFS, true, (uintptr_t) new_tempfs, 0, 0);
    printf(BGRN " Ok!\n" WHT);
}

void _start() {
    init_kernel_data();
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
    for(;;);
}
