#pragma once
#include <stdint.h>

typedef struct {
    uint16_t magic_bytes;
    uint8_t  font_mode;
    uint8_t  char_size;
} __attribute__((packed)) psf1Header;

void init_font();
void scroll_line();
void write_char(char ch);
void write_string(const char *str);
void clear_screen();