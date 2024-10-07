#include "../drivers/include/renderfont.h"
#include "../kernel/kernel.h"
#include "../drivers/include/framebuffer.h"
#include "../drivers/include/serial.h"
#include "include/string.h"
#include "include/printf.h"
#include "../processors/include/spinlock.h"
#include <stdarg.h>

Spinlock serial_lock;

void write_text(char *text) {
    write_serial(text);
    if (kernel.font_avaliable) write_string(text);
}

void write_character(char ch) {
    write_serial_char(ch);
    if (kernel.font_avaliable) write_char(ch);
}

void aquire_serial_lock() {
    spinlock_aquire(&serial_lock);
}

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
                write_text(buffer);
            } else if (format[i] == 'c') {
                char character = va_arg(args, int);
                write_character(character);
            } else if (format[i] == 'x') {
                char bufferx[20];
                ku_uint64_to_hex_string(va_arg(args, uint64_t), bufferx);
                bufferx[19] = 0;
                write_text(bufferx);
            } else if (format[i] == 'b') {
                char bufferb[65];
                ku_uint64_to_binary_string(va_arg(args, uint64_t), bufferb);
                bufferb[64] = 0;
                write_text(bufferb);
            } else if (format[i] == 's') {
                write_text(va_arg(args, char*));
            }
        } else {
            write_character(format[i]);
        }
        i++;
    }
    if (kernel.font_avaliable) swap_framebuffers();
    spinlock_release(&serial_lock);
}

void printf(char* format, ...) {
    spinlock_aquire(&serial_lock);
    va_list args;
    va_start(args, format);
    printf_template(format, args);
    va_end(args);
}

void kstatusf_helper(const char *format, ...) {
    va_list args;
    va_start(args, format);
    printf_template(format, args);
    va_end(args);
}

void k_ok() {
    spinlock_aquire(&serial_lock);
    write_text(BGRN " Ok!\n" WHT);
    spinlock_release(&serial_lock);
    if (kernel.font_avaliable) swap_framebuffers();
}