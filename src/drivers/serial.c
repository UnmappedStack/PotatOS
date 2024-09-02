// this file is largely based on the osdev wiki example cos I'm not wasting time on serial lol

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "include/serial.h"

#define PORT 0x3f8          // COM1

uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ( "inb %w1, %b0"
                   : "=a"(ret)
                   : "Nd"(port)
                   : "memory");
    return ret;
}

void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ( "outb %b0, %w1" : : "a"(val), "Nd"(port) : "memory");
}

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
   return 0;
}

static int is_transmit_empty() {
   return inb(PORT + 5) & 0x20;
}

void write_serial_char(char a) {
   while (is_transmit_empty() == 0);
   outb(PORT,a);
}

int strlen(char *str) {
    size_t i = 0;
    while (1) {
        if (str[i] == 0) break;
        i++;
    }
    return i;
}

void write_serial(char *str) {
    int len = strlen(str);
    for (int i = 0; i < len; i++) write_serial_char(str[i]);
}
