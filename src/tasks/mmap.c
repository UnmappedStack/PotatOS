#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "../kernel/kernel.h"
#include "include/mmap.h"
#include "include/tasklist.h"
#include "../mem/include/paging.h"
#include "../mem/include/pmm.h"

void* syscall_mmap(void *addr, size_t length, int prot, int flags, int fd, uint64_t offset) {
    Task *current_task = get_current_task();
    if (length <= 0) {
        *current_task->errno = 3;
        return NULL;
    }
    if (addr == NULL) {
        addr = current_task->mmap_upto;
        current_task->mmap_upto = (void*) PAGE_ALIGN_UP(((uint64_t) current_task->mmap_upto) + length);
    } 
    uint64_t map_flags = KERNEL_PFLAG_PRESENT | KERNEL_PFLAG_USER;
    if (flags & PROT_WRITE) 
        map_flags |= KERNEL_PFLAG_WRITE;
    uint64_t *buf = (uint64_t*) (((uint64_t) kmalloc(PAGE_ALIGN_UP(length) / 4096)) + kernel.hhdm);
    map_pages((uint64_t*) (current_task->pml4_addr + kernel.hhdm), (uint64_t) addr, ((uint64_t) buf) - kernel.hhdm, length, map_flags);
    if (flags & MAP_ANONYMOUS) return addr;
    File *src = current_task->resources[fd];
    read(src, (char*) buf, length);
    return addr;
}
