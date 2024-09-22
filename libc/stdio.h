#pragma once
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define stdin  0
#define stdout 1
#define stderr 2

#define DECIMAL 1
#define HEX     2

size_t strlen(const char *str);

#ifndef STRLEN_IMPL
size_t strlen(const char *str) {
    size_t len = 0;
    while (str[len])
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
        "movq %2, %%rdi\n" // file descriptor
        "movq %0, %%rsi\n" // buffer addr
        "movq %1, %%rdx\n" // buffer len
        "movq $1, %%rax\n" // write syscall
        "int $0x80"
        : : "r" ((uint64_t) str), "r" ((uint64_t) len), "r" ((uint64_t) file_descriptor)
        : "%rdi", "%rsi", "%rdx", "%rax"
    );
}
#endif

void printf(char *format, ...);

#ifndef PRINTF_IMPL
void printf(char *format, ...) {
    va_list args;
    va_start(args, format);
    size_t i   = 0;
    size_t len = strlen(format);
    while (i < len) {
        if (format[i] == '%') {
            i++;
            char buffer[10];
            if (format[i] == 'd' || format[i] == 'i') {
                int_to_string(va_arg(args, uint64_t), buffer);
                buffer[9] = 0;
                fputs(buffer, stdout);
            } else if (format[i] == 'c') {
                buffer[0] = va_arg(args, int);
                buffer[1] = 0;
                fputs(buffer, stdout);
            } else if (format[i] == 'x') {
                char bufferx[20];
                int_to_hex_string(va_arg(args, uint64_t), bufferx);
                bufferx[19] = 0;
                fputs(bufferx, stdout);
            } else if (format[i] == 's') {
                fputs(va_arg(args, char*), stdout);
            }
        } else {
            char buffer[2];
            buffer[0] = format[i];
            buffer[1] = 0;
            fputs(buffer, stdout);
        }
        i++;
    }
    va_end(args);
}
#endif