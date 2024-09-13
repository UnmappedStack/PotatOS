#include "include/framebuffer.h"
#include "../kernel/kernel.h"

void draw_pixel(int x, int y, int colour) {
    struct limine_framebuffer *framebuffer = kernel.framebuffers[0];
    volatile uint32_t *fb_ptr = framebuffer->address;
    uint32_t* location = (uint32_t*)(((uint8_t*)fb_ptr) + y * framebuffer->pitch);
    location[x] = colour;
}

void fill_screen(int colour) {
    struct limine_framebuffer *framebuffer = kernel.framebuffers[0];
    uint64_t fb_width  = framebuffer->width;
    uint64_t fb_height = framebuffer->height;
    for (uint64_t x = 0; x < fb_width; x++) {
        for (uint64_t y = 0; y < fb_height; y++) {
            draw_pixel(x, y, colour);
        }
    }
}