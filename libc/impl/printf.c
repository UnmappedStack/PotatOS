/* This is just a temporary printf implementation for debugging other libc functions such as the heap.
 * Later I plan to use stb_sprintf and have all of the printf variants.
 */

#include <stdio.h>

/* Internal printf functions first */
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

int get_num_length(uint64_t num) {
    int length = 0;
    do {
        length++;
        num /= 10;
    } while (num > 0);
    return length;
}

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
    while (index < 16)
        buffer[index++] = '0';
    buffer[index] = 0;
    reverse(buffer, index);
    memcpy(str, buffer, 17);
}

void int_to_string(uint64_t num, char *str) {
    int length = get_num_length(num);
    str[length] = '\0';
    int index = length - 1;
    do {
        str[index--] = '0' + (num % 10);
        num /= 10;
    } while (num > 0);
}

/* Then the actual printf implementation */
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
