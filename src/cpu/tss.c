#include "../utils/include/printf.h"
#include "../kernel/kernel.h"
#include "include/tss.h"
#include "../mem/include/paging.h"

void init_TSS() {
    kstatusf("Initiating TSS...");
    kernel.tss.rsp0 = KERNEL_STACK_PTR;
    printf(BGRN " Ok!\n" WHT);
}
