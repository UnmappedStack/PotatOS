#pragma once
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#define stdin  0
#define stdout 1
#define stderr 2

int fputs(const char *str, int stream);
char* fgets(char *str, int n, int fd);
void printf(const char *format, ...);
