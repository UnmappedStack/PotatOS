#pragma once
#include "acpi.h"
#include <stdint.h>

#define LOCAL_APIC 0
#define IOAPIC     1

#define LAPIC_SPURIOUS_INTERRUPT_VECTOR_REGISTER 0x0f0
#define LAPIC_DESTINATION_FORMAT_REGISTER        0x0e0
#define LAPIC_TASK_PRIORITY_REGISTER             0x080

// define MADT table entry types
typedef struct {
    uint8_t entry_type;
    uint8_t record_length;
} __attribute__ ((packed)) MADTEntryHeader;

typedef struct {
    MADTEntryHeader header;
    uint8_t processor_id;
    uint8_t apic_id;
    uint32_t flags;
} __attribute__ ((packed)) ProcessorLocalAPIC;

typedef struct {
    MADTEntryHeader header;
    uint8_t ioapic_id;
    uint8_t rsvd;
    uint32_t ioapic_addr;
    uint32_t global_system_interrupt_base;
} __attribute__ ((packed)) IOApic;

typedef struct {
    MADTEntryHeader header;
    uint8_t bus_source;
    uint8_t irq_source;
    uint32_t global_system_interrupt;
    uint16_t flags;
} __attribute__ ((packed)) IOApicInterruptSourceOverride;

typedef struct {
    MADTEntryHeader header;
    uint8_t nmi_source;
    uint8_t rsvd;
    uint16_t flags;
    uint32_t global_system_interrupt;
} __attribute__ ((packed)) IOApicNonMaskableInterruptSource;

typedef struct {
    MADTEntryHeader header;
    uint8_t processor_id;
    uint16_t flags;
    uint8_t lint;
} __attribute__ ((packed)) LocalApicNonMaskableInterrupts;

typedef struct {
    MADTEntryHeader header;
    uint16_t rsvd;
    uint64_t local_apic_addr;
} __attribute__ ((packed)) LocalApicAddressOverride;

typedef struct {
    MADTEntryHeader header;
    uint16_t rsvd;
    uint32_t local_x2apic_id;
    uint32_t flags;
    uint32_t acpi_id;
} __attribute__ ((packed)) Localx2APIC;

typedef struct {
    ISDTHeader header;
    uint32_t local_apic_addr;
    uint32_t flags;
    // fields are then here
} __attribute__ ((packed)) MADT;

void init_apic();