#pragma once
#include <syscall.h>
#include <stddef.h>
#include <stdint.h>

size_t write(int fd, const void *buf, size_t count);
size_t read(int fd, void *buf, size_t count);
char* getcwd(char *buf, size_t size);
uint64_t spawn(char *path, const char **argv, uint64_t argc);
