#include "../utils/include/printf.h"
#include "include/acpi.h"
#include "../kernel/kernel.h"
#include "../utils/include/cpu_utils.h"

void init_acpi() {
    kstatusf("Initiating ACPI...");
    if (kernel.xsdp_table->revision != 2) {
        kfailf("XSDP table not present, only RSDP (meaning that ACPI is version 1.0, must be 2.0 or higher). Cannot parse ACPI tables. Failed to boot.\n");
        halt();
    }
    k_ok();
}