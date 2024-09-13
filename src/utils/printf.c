#include "../drivers/include/framebuffer.h"
#include "../drivers/include/serial.h"
#include "include/string.h"
#include "include/printf.h"
#include <stdarg.h>

void printf_template(char* format, va_list args) {
    size_t i   = 0;
    size_t len = ku_strlen(format);
    while (i < len) {
        if (format[i] == '%') {
            i++;
            char buffer[10];
            if (format[i] == 'd' || format[i] == 'i') {
                ku_uint64_to_string(va_arg(args, uint64_t), buffer);
                buffer[9] = 0;
                write_serial(buffer);
            } else if (format[i] == 'c') {
                char character = va_arg(args, int);
                write_serial_char(character);
            } else if (format[i] == 'x') {
                char bufferx[20];
                ku_uint64_to_hex_string(va_arg(args, uint64_t), bufferx);
                bufferx[19] = 0;
                write_serial(bufferx);
            } else if (format[i] == 'b') {
                char bufferb[65];
                ku_uint64_to_binary_string(va_arg(args, uint64_t), bufferb);
                bufferb[64] = 0;
                write_serial(bufferb);
            } else if (format[i] == 's') {
                write_serial(va_arg(args, char*));
            }
        } else {
            write_serial_char(format[i]);
        }
        i++;
    }
}

void printf(char* format, ...) {
    va_list args;
    va_start(args, format);
    printf_template(format, args);
    va_end(args);
}

// and alternate versions for some different types of messages
void kstatusf(char* format, ...) {
    write_serial(BYEL "[STATUS] " WHT);
    va_list args;
    va_start(args, format);
    printf_template(format, args);
    va_end(args);
}


void ktestf(char* format, ...) {
    write_serial(BCYN "[ TEST ] " WHT);
    va_list args;
    va_start(args, format);
    printf_template(format, args);
    va_end(args);
}

void kdebugf(char* format, ...) {
    write_serial(BMAG "[KDEBUG] " WHT);
    va_list args;
    va_start(args, format);
    printf_template(format, args);
    va_end(args);
}

void kfailf(char* format, ...) {
    fill_screen(0xFF0000);
    write_serial(BRED "[ FAIL ] " WHT);
    va_list args;
    va_start(args, format);
    printf_template(format, args);
    va_end(args);
}
