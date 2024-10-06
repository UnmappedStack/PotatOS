#include <stdint.h>
#include "../utils/include/printf.h"
#include "include/framebuffer.h"
#include "../kernel/kernel.h"
#include "../mem/include/pmm.h"
#include "../mem/include/paging.h"
#include "../utils/include/string.h"

void init_framebuffer() {
    kstatusf("Initiating framebuffers...");
    struct limine_framebuffer *framebuffer = kernel.framebuffers[0];
    kernel.framebuffer_size = framebuffer->width * (framebuffer->bpp / 8) * framebuffer->height;
    kernel.back_buffer = kmalloc(PAGE_ALIGN_UP(kernel.framebuffer_size) / 4096) + kernel.hhdm;
    ku_memset((uint8_t*) kernel.back_buffer, 0, kernel.framebuffer_size);
    k_ok();
}

void swap_framebuffers() {
    struct limine_framebuffer *framebuffer = kernel.framebuffers[0];
    uint32_t *front_buffer = (void*) framebuffer->address;
    uint32_t *back_buffer  = (void*) kernel.back_buffer;
    ku_memcpy((char*) front_buffer, (char*) back_buffer, kernel.framebuffer_size);
}

void draw_pixel(uint64_t x, uint64_t y, uint32_t colour) {
    struct limine_framebuffer *framebuffer = kernel.framebuffers[0];
    volatile uint32_t *fb_ptr = kernel.back_buffer;
    uint32_t *location = (uint32_t*)(((uint8_t*)fb_ptr) + y * framebuffer->pitch);
    location[x] = colour;
}

void fill_screen(uint32_t colour) {
    struct limine_framebuffer *framebuffer = kernel.framebuffers[0];
    uint64_t fb_width  = framebuffer->width;
    uint64_t fb_height = framebuffer->height;
    for (uint64_t x = 0; x < fb_width; x++) {
        for (uint64_t y = 0; y < fb_height; y++) {
            draw_pixel(x, y, colour);
        }
    }
    swap_framebuffers();
}

void scroll_pixel(int pixels) {
    struct limine_framebuffer *framebuffer = kernel.framebuffers[0];
    volatile uint32_t *fb_ptr = kernel.back_buffer;
    for (uint64_t y = 0; y <= framebuffer->height; y++) {
        for (uint64_t x = 0; x < framebuffer->width; x++) {
            if (y >= (framebuffer->height - pixels)) {
                draw_pixel(x, y, kernel.bg_colour);
                continue;
            }
            uint32_t *location = (uint32_t*)(((uint8_t*)fb_ptr) + (y + pixels) * framebuffer->pitch);
            uint32_t above_pixel_value = location[x];
            draw_pixel(x, y, above_pixel_value);
        }
    }
}