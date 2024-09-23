#pragma once
#include <stdint.h>

void init_framebuffer();
void draw_pixel(uint64_t x, uint64_t y, uint32_t colour);
void fill_screen(uint32_t colour);
void swap_framebuffers();