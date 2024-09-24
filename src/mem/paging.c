#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "../utils/include/cpu_utils.h"
#include "include/map.h"
#include "include/pmm.h"
#include "../drivers/include/serial.h"
#include "../utils/include/string.h"
#include "../utils/include/printf.h"
#include "include/paging.h"
#include "../kernel/kernel.h"


#define PAGE_ALIGN_DOWN(addr) ((addr / 4096) * 4096)
#define PAGE_ALIGN_UP(x) ((((x) + 4095) / 4096) * 4096)

#define TOPBITS 0xFFFF000000000000

// Returns the physical address of a virtual address.
// Returns 0xDEAD if it's unmapped.
uint64_t virt_to_phys(uint64_t pml4_addr[], uint64_t virt_addr) {
    virt_addr &= ~TOPBITS;
    uint64_t pml1 = (virt_addr >> 12) & 511;
    uint64_t pml2 = (virt_addr >> (12 + 9)) & 511;
    uint64_t pml3 = (virt_addr >> (12 + 18)) & 511;
    uint64_t pml4 = (virt_addr >> (12 + 27)) & 511;
    for (; pml4 < 512; pml4++) {
        uint64_t *pml3_addr = NULL;
        if (pml4_addr[pml4] == 0)
            return 0xDEAD;
        else
            pml3_addr = (uint64_t*)(PAGE_ALIGN_DOWN(pml4_addr[pml4]) + kernel.hhdm);

        for (; pml3 < 512; pml3++) {
            uint64_t *pml2_addr = NULL;
            if (pml3_addr[pml3] == 0)
                return 0xDEAD;
            else
                pml2_addr = (uint64_t*)(PAGE_ALIGN_DOWN(pml3_addr[pml3]) + kernel.hhdm);

            for (; pml2 < 512; pml2++) {
                uint64_t *pml1_addr = NULL;
                if (pml2_addr[pml2] == 0)
                    return 0xDEAD;
                else
                    pml1_addr = (uint64_t*)(PAGE_ALIGN_DOWN(pml2_addr[pml2]) + kernel.hhdm);
                
                if (pml1_addr[pml1] == 0)
                    return 0xDEAD;

                return (uint64_t)(PAGE_ALIGN_DOWN(pml1_addr[pml1]));
            }
            pml2 = 0;
        }
        pml3 = 0;
    }
    return 0xDEAD;
}

// writes data to a location in virtual memory
void write_vmem(uint64_t *pml4_addr, uint64_t virt_addr, char *data, size_t len) {
    while (len > 0) {
        // get the address of this virtual address in kernel memory
        char *kernel_addr = (char*) virt_to_phys(pml4_addr, virt_addr) + kernel.hhdm;
        uint64_t bytes_to_copy = (len < PAGE_SIZE) ? len : PAGE_SIZE;
        ku_memcpy(kernel_addr, data, bytes_to_copy);
        if (len < 4096) return;
        len -= PAGE_SIZE;
        virt_addr += PAGE_SIZE;
        data += PAGE_SIZE;
    }
}

void map_pages(uint64_t pml4_addr[], uint64_t virt_addr, uint64_t phys_addr, uint64_t num_pages, uint64_t flags) {
    virt_addr &= ~TOPBITS;
    uint64_t pml1 = (virt_addr >> 12) & 511;
    uint64_t pml2 = (virt_addr >> (12 + 9)) & 511;
    uint64_t pml3 = (virt_addr >> (12 + 18)) & 511;
    uint64_t pml4 = (virt_addr >> (12 + 27)) & 511;
    for (; pml4 < 512; pml4++) {
        uint64_t *pml3_addr = NULL;
        if (pml4_addr[pml4] == 0) {
            pml4_addr[pml4] = (uint64_t)kmalloc(1);
            pml3_addr = (uint64_t*)(pml4_addr[pml4] + kernel.hhdm);
            ku_memset((uint8_t*)pml3_addr, 0, 4096);
            pml4_addr[pml4] |= flags | KERNEL_PFLAG_PRESENT | KERNEL_PFLAG_WRITE;
        } else {
            pml3_addr = (uint64_t*)(PAGE_ALIGN_DOWN(pml4_addr[pml4]) + kernel.hhdm);
        }
        
        for (; pml3 < 512; pml3++) {
            uint64_t *pml2_addr = NULL;
            if (pml3_addr[pml3] == 0) {
                pml3_addr[pml3] = (uint64_t)kmalloc(1);
                pml2_addr = (uint64_t*)(pml3_addr[pml3] + kernel.hhdm);
                ku_memset((uint8_t*)pml2_addr, 0, 4096);
                pml3_addr[pml3] |= flags | KERNEL_PFLAG_PRESENT | KERNEL_PFLAG_WRITE;
            } else {
                pml2_addr = (uint64_t*)(PAGE_ALIGN_DOWN(pml3_addr[pml3]) + kernel.hhdm);
            }

            for (; pml2 < 512; pml2++) {
                uint64_t *pml1_addr = NULL;
                if (pml2_addr[pml2] == 0) {
                    pml2_addr[pml2] = (uint64_t)kmalloc(1);
                    pml1_addr = (uint64_t*)(pml2_addr[pml2] + kernel.hhdm);
                    ku_memset((uint8_t*)pml1_addr, 0, 4096);
                    pml2_addr[pml2] |= flags | KERNEL_PFLAG_PRESENT | KERNEL_PFLAG_WRITE;
                } else {
                    pml1_addr = (uint64_t*)(PAGE_ALIGN_DOWN(pml2_addr[pml2]) + kernel.hhdm);
                }
                for (; pml1 < 512; pml1++) {
                    pml1_addr[pml1] = phys_addr | flags;
                    num_pages--;
                    phys_addr += 4096;
                    if (num_pages == 0) return;
                }
                pml1 = 0;
            }
            pml2 = 0;
        }
        pml3 = 0;
    }
    printf(BRED "\n[KPANIC] " WHT "Failed to allocate pages: No more avaliable virtual memory. Halting.\n");
    halt();
} 

void alloc_pages(uint64_t pml4_addr[], uint64_t virt_addr, uint64_t num_pages, uint64_t flags) {
    virt_addr &= ~TOPBITS;
    uint64_t pml1 = (virt_addr >> 12) & 511;
    uint64_t pml2 = (virt_addr >> (12 + 9)) & 511;
    uint64_t pml3 = (virt_addr >> (12 + 18)) & 511;
    uint64_t pml4 = (virt_addr >> (12 + 27)) & 511;
    for (; pml4 < 512; pml4++) {
        uint64_t *pml3_addr = NULL;
        if (pml4_addr[pml4] == 0) {
            pml4_addr[pml4] = (uint64_t)kmalloc(1);
            pml3_addr = (uint64_t*)(pml4_addr[pml4] + kernel.hhdm);
            ku_memset((uint8_t*)pml3_addr, 0, 4096);
            pml4_addr[pml4] |= flags | KERNEL_PFLAG_PRESENT | KERNEL_PFLAG_WRITE;
        } else {
            pml3_addr = (uint64_t*)(PAGE_ALIGN_DOWN(pml4_addr[pml4]) + kernel.hhdm);
        }
        for (; pml3 < 512; pml3++) {
            uint64_t *pml2_addr = NULL;
            if (pml3_addr[pml3] == 0) {
                pml3_addr[pml3] = (uint64_t)kmalloc(1);
                pml2_addr = (uint64_t*)(pml3_addr[pml3] + kernel.hhdm);
                ku_memset((uint8_t*)pml2_addr, 0, 4096);
                pml3_addr[pml3] |= flags | KERNEL_PFLAG_PRESENT | KERNEL_PFLAG_WRITE;
            } else {
                pml2_addr = (uint64_t*)(PAGE_ALIGN_DOWN(pml3_addr[pml3]) + kernel.hhdm);
            }

            for (; pml2 < 512; pml2++) {
                uint64_t *pml1_addr = NULL;
                if (pml2_addr[pml2] == 0) {
                    pml2_addr[pml2] = (uint64_t)kmalloc(1);
                    pml1_addr = (uint64_t*)(pml2_addr[pml2] + kernel.hhdm);
                    ku_memset((uint8_t*)pml1_addr, 0, 4096);
                    pml2_addr[pml2] |= flags | KERNEL_PFLAG_PRESENT | KERNEL_PFLAG_WRITE;
                } else {
                    pml1_addr = (uint64_t*)(PAGE_ALIGN_DOWN(pml2_addr[pml2]) + kernel.hhdm);
                }
                
                for (; pml1 < 512; pml1++) {
                    uint64_t phys = (uint64_t)kmalloc(1);
                    pml1_addr[pml1] = phys | flags;
                    num_pages--;
                    if (num_pages == 0) return;
                }
                pml1 = 0;
            }
            pml2 = 0;
        }
        pml3 = 0;
    }
    kfailf("Failed to allocate pages: No more avaliable virtual memory. Halting.\n");
    halt();
}

uint64_t* init_paging_task() {
    uint64_t pml4_virt = ((uint64_t) kmalloc(1)) + kernel.hhdm;
    ku_memset((uint8_t*) pml4_virt, 0, 4096);
    map_all((uint64_t*) pml4_virt);
    return (uint64_t*) pml4_virt;
}

void init_paging() {
    kstatusf("Creating page tree... ");
    uint64_t pml4_virt = ((uint64_t) kmalloc(1)) + kernel.hhdm;
    ku_memset((uint8_t*) pml4_virt, 0, 4096);
    map_all((uint64_t*) pml4_virt);
    kernel.cr3 = pml4_virt - kernel.hhdm;
    k_ok();
}
