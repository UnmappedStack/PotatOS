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

uint32_t read_ioapic(void *ioapic_addr, uint32_t reg) {
   uint32_t volatile *ioapic = (uint32_t volatile *)ioapic_addr;
   ioapic[0] = (reg & 0xff);
   return ioapic[4];
}

void write_ioapic(void *ioapic_addr, uint32_t reg, uint32_t value) {
   uint32_t volatile *ioapic = (uint32_t volatile *)ioapic_addr;
   ioapic[0] = (reg & 0xff);
   ioapic[4] = value;
}

void map_ioapic(uintptr_t ioapic_addr, uint8_t vec, uint32_t gsi_base, uint32_t irq, uint32_t lapic_id) {
    kstatusf("Mapping vector %i to irq %i on lapic ID %i\n", vec, irq, lapic_id);
    kdebugf("Global system interrupt base: %i\n", gsi_base);
    uint32_t entry_num = gsi_base + (irq * 2);
    kdebugf("Entry number: %i\n", entry_num);
    uint32_t reg_nums[2] = {0x10 + entry_num, 0x11 + entry_num};
    kdebugf("Register numbers: %i and %i\n", reg_nums[0], reg_nums[1]);
    uint32_t redirection_entries[2] = {read_ioapic((void*) ioapic_addr, reg_nums[0]), read_ioapic((void*) ioapic_addr, reg_nums[1])};
    kdebugf("Original redirection entries: %i and %i\n", redirection_entries[0], redirection_entries[1]);
    kstatusf("Trying to set entry one...\n");
    redirection_entries[0] = (redirection_entries[0] & ~0xFF) | vec; // set vector number
    redirection_entries[0] &= ~0x300; // set delivery mode to normal
    redirection_entries[0] &= ~0x400; // set destination mode to physical. Probably worse but for now it's just easier.
    redirection_entries[0] &= ~0x1000; // set polarity to high
    redirection_entries[0] &= ~0x8000; // makes sure that it's unmasked
    kdebugf("Done! New value: %i\n", redirection_entries[0]);
    kstatusf("Trying to set entry two...\n");
    redirection_entries[1] = (lapic_id & 0xF) << 28;
    kdebugf("Done: New value: %i\n", redirection_entries[1]);
    kstatusf("Trying to set new entries...\n");
    write_ioapic((void*) ioapic_addr, reg_nums[0], redirection_entries[0]);
    write_ioapic((void*) ioapic_addr, reg_nums[1], redirection_entries[1]);
    kstatusf("Done, this IOAPIC IRQ has been mapped and unmasked.\n");
}

void init_local_apic(uintptr_t lapic_addr) {
    kdebugf("Local APIC vaddr: 0x%x\n", lapic_addr);
    kstatusf("Setting task priority of LAPIC...\n");
    write_lapic(lapic_addr, LAPIC_TASK_PRIORITY_REGISTER, 0);
    kstatusf("Setting LAPIC destination format to flat mode...\n");
    write_lapic(lapic_addr, LAPIC_DESTINATION_FORMAT_REGISTER, read_lapic(lapic_addr, LAPIC_DESTINATION_FORMAT_REGISTER) | 0xF0000000);
    kstatusf("Setting spurious interrupt vector (and enabling this LAPIC)...\n");
    write_lapic(lapic_addr, LAPIC_SPURIOUS_INTERRUPT_VECTOR_REGISTER, 0x40 | 0x80);
    kstatusf("This LAPIC was successfully set up!\n");
}

void init_apic() {
    kstatusf("Initiating APIC...\n");
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
            printf(" - Global system interrupt base: %i\n", this_ioapic->global_system_interrupt_base);
            map_pages((uint64_t*) (kernel.cr3 + kernel.hhdm), (uint64_t) this_ioapic->ioapic_addr + kernel.hhdm, (uint64_t) this_ioapic->ioapic_addr, 1, KERNEL_PFLAG_PRESENT | KERNEL_PFLAG_WRITE);
            ktestf("Attempting to map irq on this IO APIC...\n");
            map_ioapic((uintptr_t) (((uint64_t) this_ioapic->ioapic_addr) + kernel.hhdm), 33, this_ioapic->global_system_interrupt_base, 1, 0);
        } else if (entry->entry_type == LOCAL_APIC) {
            ProcessorLocalAPIC *this_local_apic = (ProcessorLocalAPIC*) entry;
            kstatusf("Processor local APIC device found. Information:\n");
            printf(" - Processor ID: %i\n", this_local_apic->processor_id);
            printf(" - APIC ID: %i\n", this_local_apic->apic_id);
        }
        entry = (MADTEntryHeader*) (((uint64_t) entry) + entry->record_length);
        incremented += entry->record_length;
    }
    //enable_interrupts();
    //for (;;);
}