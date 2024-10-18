#pragma once
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define MAP_ANONYMOUS 0x20
#define PROT_WRITE    2

#define stdin  0
#define stdout 1
#define stderr 2

#define DECIMAL 1
#define HEX     2

size_t strlen(const char *str);

#ifndef STRLEN_IMPL
size_t strlen(const char *str) {
    size_t len = 0;
    while (str[len] != 0)
        len++;
    return len;
} 
#endif

void reverse(char str[], int length);

#ifndef REVERSE_IMPL
void reverse(char str[], int length) {
    int start = 0;
    int end = length - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}
#endif

void memcpy(char* dest, const char* from, size_t n);

#ifndef MEMCPY_IMPL
void memcpy(char* dest, const char* from, size_t n) {
    for (size_t i = 0; i < n; i++)
        dest[i] = from[i];
}
#endif

int get_num_length(uint64_t num);

#ifndef GET_NUM_LENGTH_IMPL
int get_num_length(uint64_t num) {
    int length = 0;
    do {
        length++;
        num /= 10;
    } while (num > 0);
    return length;
}
#endif

void int_to_hex_string(uint64_t num, char *str);

#ifndef INT_TO_HEX_STR_IMPL
void int_to_hex_string(uint64_t num, char *str) {
    char buffer[17];
    int index = 0;
    if (num == 0) {
        buffer[index++] = '0';
    } else {
        while (num > 0) {
            uint8_t digit = num & 0xF;
            if (digit < 10) {
                buffer[index++] = '0' + digit;
            } else {
                buffer[index++] = 'A' + (digit - 10);
            }
            num >>= 4;
        }
    }
    // Pad the buffer with 0s to make sure it's 16 digits
    while (index < 16)
        buffer[index++] = '0';
    buffer[index] = '\0'; // Null-terminate the string
    reverse(buffer, index); // Reverse the string
    memcpy(str, buffer, 17); // Copy the final result to the destination
}
#endif

void int_to_string(uint64_t num, char *str);

#ifndef INT_TO_STR_IMPL
void int_to_string(uint64_t num, char *str) {
    // Get the length of the number
    int length = get_num_length(num);
    // Null-terminate the string
    str[length] = '\0';
    // Fill the buffer with digits in reverse order
    int index = length - 1;
    do {
        str[index--] = '0' + (num % 10);
        num /= 10;
    } while (num > 0);
}
#endif

void fputs(char *str, uint8_t file_descriptor);

#ifndef FPUTS_IMPL
void fputs(char *str, uint8_t file_descriptor) {
    size_t len = strlen(str);
    asm volatile (
        "int $0x80"
        : : "S" ((uint64_t) str), "d" ((uint64_t) len), "D" ((uint64_t) file_descriptor), "a" (1)
    );
}
#endif

void write(char *str, uint8_t file_descriptor, uint64_t len);

#ifndef WRITE_IMPL
void write(char *str, uint8_t file_descriptor, uint64_t len) {
    asm volatile (
        "int $0x80"
        : : "S" ((uint64_t) str), "d" (len), "D" ((uint64_t) file_descriptor), "a" (1)
    );
}
#endif

void fgets(char *buffer, uint64_t max_len, uint8_t file_descriptor);

#ifndef FGETS_IMPL
void fgets(char *buffer, uint64_t max_len, uint8_t file_descriptor) {
    asm volatile (
        "int $0x80"
        : : "S" ((uint64_t) buffer), "d" ((uint64_t) max_len), "D" ((uint64_t) file_descriptor), "a" (0)
    );
}
#endif

void printf(const char *format, ...);

#ifndef PRINTF_IMPL
void printf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    while (*format) {
        if (*format == '%') {
            format++;
            char buffer[10];
            if (*format == 'd' || *format == 'i') {
                int_to_string(va_arg(args, uint64_t), buffer);
                buffer[9] = 0;
                fputs(buffer, stdout);
            } else if (*format == 'c') {
                buffer[0] = va_arg(args, int);
                buffer[1] = 0;
                fputs(buffer, stdout);
            } else if (*format == 'x') {
                char bufferx[20];
                int_to_hex_string(va_arg(args, uint64_t), bufferx);
                bufferx[19] = 0;
                fputs(bufferx, stdout);
            } else if (*format == 's') {
                fputs(va_arg(args, char*), stdout);
            }
        } else {
            char buffer[2];
            buffer[0] = *format;
            buffer[1] = 0;
            fputs(buffer, stdout);
        }
        format++;
    }
    va_end(args);
}
#endif

#define O_CREAT    0b001
#define O_CREATALL 0b010

#define MODE_READONLY  0b01
#define MODE_WRITEONLY 0b10
#define MODE_READWRITE 0b11

#define File int

File open(char *path, uint64_t flags, uint64_t mode);

#ifndef OPEN_IMPL
File open(char *path, uint64_t flags, uint64_t mode) {
    int file_descriptor;
    asm volatile (
        "int $0x80"
        : "=a" (file_descriptor)
        : "D" ((uint64_t) path), "S" ((uint64_t) flags), "d" ((uint64_t) mode), "a" (2)
    );
    return file_descriptor;
}
#endif

void close(File f);

#ifndef CLOSE_IMPL
void close(File f) {
    asm volatile (
        "int $0x80"
        : : "D" ((uint64_t) f), "a" (3)
    );
}
#endif

void exit();

#ifndef EXIT_IMPL
void exit() {
    asm volatile(
        "int $0x80"
        : : "a" (9)
    );
}
#endif

uint64_t spawn(char *path, const char **argv, uint64_t argc);

#ifndef SPAWN_IMPL
uint64_t spawn(char *path, const char **argv, uint64_t argc) {
    uint64_t status;
    asm volatile (
        "int $0x80"
        : "=a" (status)
        : "D" ((uint64_t) path), "S" ((uint64_t) argv), "d" (argc), "a" (4)
    );
    return status;
}
#endif

void* mmap(void *addr, size_t length, int prot, int flags, int fd, uint64_t offset);

#ifndef MMAP_IMPL
void* mmap(void *addr, size_t length, int prot, int flags, int fd, uint64_t offset) {
    void *result;
    asm volatile (
        "mov %5, %%r8\n"
        "mov %6, %%r9\n"
        "int $0x80"
        : "=a" (result)
        : "D" (addr), "S" (length), "d" (prot), "c" (flags), "r" ((uint64_t) fd), "r" (offset), "a" (10)
    );
    return result;
}
#endif

uint64_t* get_errno_addr();

#ifndef GET_ERRNO_IMPL
uint64_t* get_errno_addr() {
    uint64_t addr;
    asm volatile (
        "int $0x80"
        : "=a" (addr)
        : "a" (8)
    );
    return (uint64_t*) addr;
}
#endif

void get_cwd(char *buffer, uint64_t buffer_len);

#ifndef GET_CWD_IMPL
void get_cwd(char *buffer, uint64_t buffer_len) {
    asm volatile (
        "int $0x80"
        : : "D" ((uint64_t) buffer), "S" ((uint64_t) buffer_len), "a" (5)
    );
}
#endif
