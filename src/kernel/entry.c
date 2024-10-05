#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "../drivers/include/keyboard.h"
#include "../tasks/include/events.h"
#include "../drivers/include/acpi.h"
#include "../drivers/include/renderfont.h"
#include "../fs/include/devices.h"
#include "../tasks/include/syscalls.h"
#include "../utils/include/cpu_utils.h"
#include "../tasks/include/spawn.h"
#include "../tasks/include/tasklist.h"
#include "../utils/include/io.h"
#include "../processors/include/smp.h"
#include "../drivers/include/pit.h"
#include "../drivers/include/irq.h"
#include "../mem/include/vector.h"
#include "../fs/include/ustar.h"
#include "../drivers/include/framebuffer.h"
#include "../fs/include/vfs.h"
#include "../fs/include/tempfs.h"
#include "../mem/include/kheap.h"
#include "../mem/include/print_mem.h"
#include "../mem/include/paging.h"
#include "../cpu/include/idt.h"
#include "../utils/include/string.h"
#include "../cpu/include/gdt.h"
#include "../limine.h"
#include "kernel.h"
#include "../drivers/include/serial.h"
#include "../utils/include/printf.h"
#include "../mem/include/pmm.h"
#include "bootinfo.h"

#if UINT32_MAX == UINTPTR_MAX
#define STACK_CHK_GUARD 0xe2dee396
#else
#define STACK_CHK_GUARD 0x595e9fbd94fda766
#endif

uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

__attribute__((noreturn))
void __stack_chk_fail(void) {
	kfailf("Stack smashing detected. Halting.\n");
    halt();
}

void init_kernel_data() {
    kernel.memmap          = *memmap_request.response;
    kernel.hhdm            = (hhdm_request.response)->offset;
    kernel.kernel_addr     = *(kernel_address_request.response);
    kernel.framebuffers    = (framebuffer_request.response)->framebuffers;
    kernel.initial_ramdisk = *((initrd_request.response)->modules);
    kernel.rsdp_table      = (RSDP*) (rsdp_request.response)->address;
    kernel.smp_response    = smp_request.response;
    kernel.font_avaliable  = false;
    kernel.ch_X            = 5;
    kernel.ch_Y            = 5;
    kernel.fg_colour       = 0xd8d9d7;
    kernel.bg_colour       = 0x012456;
}

Kernel kernel = {0};

void show_boot_info() {
    kdebugf("Higher Half Direct Mapping (HHDM): 0x%x\n", kernel.hhdm);
    kdebugf("Number of processors detected: %i\n", kernel.smp_response->cpu_count);
    kdebugf("Memory map recieved from bootloader:");
    print_memory();
    printf("\n");
}

const char *argv[] = {"R:/exec/shell", "Arg test 1!", "Second :D"};

void _start() {
    disable_interrupts();
    init_kernel_data();
    init_serial();
    show_boot_info();
    init_PMM();
    init_kheap();
    init_vfs();
    init_devices();
    setup_initrd();
    init_framebuffer();
    fill_screen(kernel.bg_colour);
    init_TSS();
    init_GDT();
    init_IDT();
    init_font();
    init_tasklist();
    init_paging();
    switch_page_structures();
    init_irq();
    init_PIT();
    init_acpi();
    init_syscalls();
    init_ps2_keyboard();
    init_smp();
    for (uint64_t i = 0; i < 9999; i++)
        outb(0x80, 0);
    kstatusf("Trying to run init process...\n");
    if (spawn("R:/exec/shell", argv, 3) != 0) {
        kfailf("Could not run init application. Halting.\n");
        halt();
    }
    clear_screen();
    enable_interrupts();
    unlock_pit();
    for(;;);
}
void all_tasks_ended() {
    disable_interrupts();
    lock_pit();
    kstatusf("All tasks halted, nothing left to do.\n\n");
    for (;;);
}
