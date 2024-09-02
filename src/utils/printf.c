#include "../drivers/include/serial.h"
#include "include/string.h"
#include "include/printf.h"
#include <stdarg.h>

void printf(char* format, ...) {
    va_list args;
    va_start(args, format);
    size_t i = 0;
    while (i < ku_strlen(format)) {
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
                bufferx[19] = 0;
                ku_uint64_to_hex_string(va_arg(args, uint64_t), bufferx);
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
    va_end(args);
}
