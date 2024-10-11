#include <stddef.h>
#include <stdint.h>
#include "../drivers/include/pit.h"
#include "../drivers/include/apic.h"
#include "../mem/include/paging.h"
#include "../utils/include/cpu_utils.h"
#include "include/smp.h"
#include "../kernel/kernel.h"
#include "../utils/include/printf.h"
#include "../limine.h"
#include "../cpu/include/gdt.h"

Spinlock init_lock;

__attribute__ ((noinline))
void ap_entry(struct limine_smp_info *smp_info) {
    disable_interrupts();
    kstatusf("Started core (PID: %i), starting initialisation.\n", smp_info->processor_id);
    kstatusf("Setting this processor's GDT...", &kernel.gdtr);
    create_system_segment_descriptor((uint64_t*) kernel.gdtr.offset, 5, (uint64_t) &kernel.tss, sizeof(struct TSS) - 1, 0x89, 0);
    asm("lgdt (%0)" : : "r" (&kernel.gdtr));
    asm volatile("push $0x08; \
              lea .reload_CS(%%rip), %%rax; \
              push %%rax; \
              retfq; \
              .reload_CS: \
              mov $0x10, %%ax; \
              mov %%ax, %%ds; \
              mov %%ax, %%es; \
              mov %%ax, %%fs; \
              mov %%ax, %%gs; \
              mov %%ax, %%ss" : : : "eax", "rax");
    k_ok();
    kdebugf("Loading this processor's TSS...");
    asm volatile("mov $0x28, %%ax\nltr %%ax" : : : "eax");
    k_ok();
    kstatusf("Setting this processor's IDT...");
    asm("lidt %0" : : "m" (kernel.idtr));
    k_ok();
    kstatusf("Initiating paging for this processor...");
    uint64_t this_ap_stack_addr = KERNEL_STACK_ADDR - (PAGE_SIZE * KERNEL_STACK_PAGES * smp_info->processor_id);
    uint64_t this_ap_stack_ptr  = KERNEL_STACK_PTR  - (PAGE_SIZE * KERNEL_STACK_PAGES * smp_info->processor_id);
    KERNEL_SWITCH_PAGE_TREE(kernel.cr3);
    alloc_pages((uint64_t*)(kernel.cr3 + kernel.hhdm), this_ap_stack_addr, KERNEL_STACK_PAGES, KERNEL_PFLAG_PRESENT | KERNEL_PFLAG_WRITE);
    KERNEL_SWITCH_STACK(this_ap_stack_ptr);
    k_ok();
    kstatusf("Setting up this processor's local APIC...\n");
    init_local_apic(kernel.lapic_addr);
    kstatusf("Setting up this processor's LAPIC timer...\n");
    init_lapic_timer();
    kstatusf("Successfully initialised processor %i.\n", smp_info->processor_id);
    spinlock_release(&init_lock);
    halt();
}

void init_smp() {
    kstatusf("Initiating & testing SMP...\n");
    for (size_t i = 1; i < kernel.smp_response->cpu_count; i++) {
        spinlock_aquire(&init_lock);
        (*kernel.smp_response->cpus)[i].goto_address = &ap_entry;
    }
    spinlock_aquire(&init_lock); // this will never be released. It's just so that this thread won't continue until the last core is finished initialising.
    kstatusf("All processors initialised.\n");
}