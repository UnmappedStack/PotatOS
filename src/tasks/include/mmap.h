#pragma once
#include <stdint.h>
#include <stddef.h>

#define MAP_ANONYMOUS 0x20
#define PROT_WRITE    2

void* syscall_mmap(void *addr, size_t length, int prot, int flags, int fd, uint64_t offset);
