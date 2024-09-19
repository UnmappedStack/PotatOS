#include <stddef.h>
#include <stdint.h>
#include "../utils/include/cpu_utils.h"
#include "include/smp.h"
#include "../kernel/kernel.h"
#include "../utils/include/printf.h"
#include "../limine.h"

void smp_test(struct limine_smp_info *smp_info) {
    printf("This message was printed from an AP (Processor ID: %i). Freezing this core. :D\n", smp_info->processor_id);
    halt();
}

void init_smp() {
    kstatusf("Initiating & testing SMP...\n");
    for (size_t i = 1; i < kernel.smp_response->cpu_count; i++)
        (*kernel.smp_response->cpus)[i].goto_address = &smp_test;
}