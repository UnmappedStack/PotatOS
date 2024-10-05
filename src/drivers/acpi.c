#include "../utils/include/string.h"
#include "../utils/include/printf.h"
#include "include/acpi.h"
#include "../kernel/kernel.h"
#include "../mem/include/paging.h"
#include "../utils/include/cpu_utils.h"

void init_acpi() {
    kstatusf("Initiating ACPI...\n");
    kdebugf("Mapping RSDP...\n");
    map_pages((uint64_t*) (kernel.cr3 + kernel.hhdm), (uint64_t) kernel.rsdp_table, ((uint64_t) kernel.rsdp_table) - kernel.hhdm, 1, KERNEL_PFLAG_PRESENT);
    if (kernel.rsdp_table->revision != 0) {
        kfailf("This device uses XSDP, but only RSDP is supported. Halting.\n");
        halt();
    }
    kdebugf("Nice! RSDP is being used. Mapping RSDT...\n");
    map_pages((uint64_t*) (kernel.cr3 + kernel.hhdm), (uint64_t) kernel.rsdp_table->rsdt_address + kernel.hhdm, (uint64_t) kernel.rsdp_table->rsdt_address, 1, KERNEL_PFLAG_PRESENT);
    RSDT *rsdt = (RSDT*) (kernel.rsdp_table->rsdt_address + kernel.hhdm);
    uint64_t rsdt_num_entries = (rsdt->header.length - sizeof(rsdt->header)) / 4;
    kdebugf("Success! RSDT virtual address: 0x%x\n", rsdt);
    kdebugf("Num RSDT entries: %i\n", rsdt_num_entries);
    kernel.rsdt = rsdt;
}

void* find_MADT(RSDT *root_rsdt) {
    uint64_t num_entries = (root_rsdt->header.length - sizeof(root_rsdt->header)) / 4;
    for (size_t i = 0; i < num_entries; i++) {
        RSDT *this_rsdt = (RSDT*) (root_rsdt->entries[i] + kernel.hhdm);
        if(ku_memcmp(this_rsdt->header.signature, "APIC", 4)) return (void*) &this_rsdt->header;
    }
}