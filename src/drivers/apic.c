#include "../utils/include/cpu_utils.h"
#include "../mem/include/paging.h"
#include "../utils/include/io.h"
#include <stddef.h>
#include <stdint.h>
#include "include/apic.h"
#include "include/acpi.h"
#include "include/irq.h"
#include "../utils/include/printf.h"
#include "../kernel/kernel.h"

void mask_pic() {
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0xFF);
    outb(0xA1, 0xFF); 
}

uint32_t read_lapic(uintptr_t lapic_addr, uint64_t reg_offset) {
    uint32_t volatile *lapic_register_addr = (uint32_t volatile *) (((uint64_t)lapic_addr) + reg_offset);
    return (uint32_t) *lapic_register_addr;
}

void write_lapic(uintptr_t lapic_addr, uint64_t reg_offset, uint32_t val) {
    uint32_t volatile *lapic_register_addr = (uint32_t volatile *) (((uint64_t)lapic_addr) + reg_offset);
    *lapic_register_addr = val;
}

void init_local_apic(uintptr_t lapic_addr) {
    kdebugf("Local APIC vaddr: 0x%x\n", lapic_addr);
    kstatusf("Setting task priority of LAPIC...\n");
    write_lapic(lapic_addr, LAPIC_TASK_PRIORITY_REGISTER, 0);
    kstatusf("Setting LAPIC destination format to flat mode...\n");
    write_lapic(lapic_addr, LAPIC_DESTINATION_FORMAT_REGISTER, 0xF0000000);
    kstatusf("Setting spurious interrupt vector (and enabling this LAPIC)...\n");
    write_lapic(lapic_addr, LAPIC_SPURIOUS_INTERRUPT_VECTOR_REGISTER, 0x40 | 0x80);
    kstatusf("This LAPIC was successfully set up!\n");
}

void init_apic() {
    mask_pic();
    MADT *madt = (MADT*) find_MADT(kernel.rsdt);
    kdebugf("Local APIC paddr: 0x%x\n", madt->local_apic_addr);
    // map the lapic addr
    map_pages((uint64_t*) (kernel.cr3 + kernel.hhdm), (uint64_t) madt->local_apic_addr + kernel.hhdm, (uint64_t) madt->local_apic_addr, 1, KERNEL_PFLAG_PRESENT | KERNEL_PFLAG_WRITE);
    uint64_t lapic_registers_virt = (uint64_t) madt->local_apic_addr + kernel.hhdm;
    //init_local_apic(lapic_registers_virt);
    MADTEntryHeader *entry = (MADTEntryHeader*) (((uint64_t) madt) + sizeof(MADT));
    uint64_t incremented = sizeof(MADT);
    kdebugf("Enumerating MADT entries...\n");
    while (incremented < madt->header.length) {
        if (entry->entry_type == IOAPIC) {
            IOApic *this_ioapic = (IOApic*) entry;
            kstatusf("I/O APIC device found. Information:\n");
            printf(" - I/O APIC ID: %i\n", this_ioapic->ioapic_id);
            printf(" - I/O APIC address: 0x%x\n", this_ioapic->ioapic_addr);
            printf(" - Global system interrupt base: 0x%x\n", this_ioapic->global_system_interrupt_base);
        } else if (entry->entry_type == LOCAL_APIC) {
            ProcessorLocalAPIC *this_local_apic = (ProcessorLocalAPIC*) entry;
            kstatusf("Processor local APIC device found. Information:\n");
            printf(" - Processor ID: %i\n", this_local_apic->processor_id);
            printf(" - APIC ID: %i\n\n", this_local_apic->apic_id);
        }
        entry = (MADTEntryHeader*) (((uint64_t) entry) + entry->record_length);
        incremented += entry->record_length;
    }
}