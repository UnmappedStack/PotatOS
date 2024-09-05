#include "../utils/include/string.h"
#include "include/paging.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "include/kheap.h"
#include "../kernel/kernel.h"
#include "include/pmm.h"
#include "../utils/include/printf.h"

typedef struct {
    uint64_t  size;
    uint64_t  required_size;
    uintptr_t next_pool;
    bool      free;
    uint8_t   data[0];
} Pool;

Pool create_pool(uint64_t size, uint64_t required_size, uintptr_t next_pool, bool free) {
    Pool pool;
    pool.size          = size;
    pool.required_size = required_size;
    pool.next_pool     = next_pool;
    pool.free          = free;
    return pool;
}

void init_kheap() {
    kstatusf("Initiating kheap...");
    kernel.kheap_start = ((uintptr_t) kmalloc(2)) + ((uintptr_t) kernel.hhdm);
    *((Pool*) kernel.kheap_start) = create_pool(4096, sizeof(Pool), 0, true);
    printf(BGRN " Ok!\n" WHT);
}

void* split_pool(Pool *pool_addr, uint64_t size) {
    Pool *new_pool         = (Pool*) (((uint64_t) pool_addr) + pool_addr->required_size);
    uint64_t new_pool_size = sizeof(Pool) + size;
    *new_pool = create_pool(pool_addr->size - pool_addr->required_size, new_pool_size, pool_addr->next_pool, false);
    pool_addr->size      = pool_addr->required_size;
    pool_addr->next_pool = (uintptr_t) new_pool;
    return (void*)new_pool;
}

void* malloc(uint64_t size) {
    Pool *this_pool = (Pool*) kernel.kheap_start;
    for (;;) {
        if (this_pool->free) {
            this_pool->free = false;
            this_pool->required_size = size + sizeof(Pool);
            return (void*) this_pool;
        } else if (this_pool->size > this_pool->required_size + size) {
            return split_pool(this_pool, size);
        } else if (this_pool->next_pool == 0) {
            uint64_t new_pool_size = PAGE_ALIGN_UP(size);
            this_pool->next_pool = ((uintptr_t) kmalloc(new_pool_size)) + ((uintptr_t) kernel.hhdm);
            *((Pool*) this_pool->next_pool) = create_pool(new_pool_size, size + sizeof(Pool), 0, false);
            return (void*) this_pool->next_pool;
        }
        this_pool = (Pool*) this_pool->next_pool;
    }
}

void free(void* addr) {
    Pool *this_pool          = (Pool*) addr;
    ku_memset(this_pool->data, 0, this_pool->size - sizeof(Pool));
    this_pool->free          = true;
    this_pool->required_size = sizeof(Pool);
}
