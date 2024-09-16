// this file is largely based on the osdev wiki example cos I'm not wasting time on serial lol

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "include/serial.h"
#include "../utils/include/string.h"
#include "../utils/include/printf.h"
#include "../utils/include/io.h"

#define PORT 0x3f8          // COM1

int init_serial() {
    outb(PORT + 1, 0x00);
    outb(PORT + 3, 0x80);
    outb(PORT + 0, 0x03);
    outb(PORT + 1, 0x00);
    outb(PORT + 3, 0x03);
    outb(PORT + 2, 0xC7);
    outb(PORT + 4, 0x0B);
    outb(PORT + 4, 0x1E);
    outb(PORT + 0, 0xAE);
    if(inb(PORT + 0) != 0xAE) {
        return 1;
    }
    outb(PORT + 4, 0x0F);
    printf("\n");
    kstatusf("Serial output initialised.\n");
    return 0;
}

static int is_transmit_empty() {
    return inb(PORT + 5) & 0x20;
}

void write_serial_char(char a) {
    while (is_transmit_empty() == 0);
    outb(PORT,a);
}

void write_serial(char *str) {
    int len = ku_strlen(str);
    for (int i = 0; i < len; i++) write_serial_char(str[i]);
}
