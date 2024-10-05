#include <stddef.h>
#include "include/apic.h"
#include "include/acpi.h"
#include "../utils/include/printf.h"
#include "../kernel/kernel.h"

void init_apic() {
    kdebugf("Enumerating MADT entries...\n");
    MADT *madt = (MADT*) find_MADT(kernel.rsdt);
    IOApic *entry = (IOApic*) (((uint64_t) madt) + sizeof(MADT));
    uint64_t incremented = sizeof(MADT);
    while (incremented < madt->header.length) {
        printf("Entry type: %i, record length: %i\n", (uint32_t) entry->entry_type, entry->record_length);
        entry = (IOApic*) (((uint64_t) entry) + entry->record_length);
        incremented += entry->record_length;
    }
}