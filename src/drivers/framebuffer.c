#include "../mem/include/kheap.h"
#include <stdint.h>
#include "../utils/include/printf.h"
#include "include/framebuffer.h"
#include "../kernel/kernel.h"
#include "../mem/include/pmm.h"
#include "../mem/include/paging.h"
#include "../utils/include/string.h"
#include "../fs/include/vfs.h"
#include "../fs/include/tempfs.h"
#include <stddef.h>

// this just needs to exist, but there really isn't anything to do
void open_fb_device(void *filev, uint8_t mode) {}
void close_fb_device(void *filev) {};
void close_fbinfo_device(void *filev) {}

void open_fbinfo_device(void *filev, uint8_t mode) {
    if (mode != MODE_READONLY) {
        kfailf("Can only open framebuffer info devices as MODE_READONLY.\n");
        return;
    }
}

int read_fbinfo_device(void *filev, char *buffer, size_t max_len) {
    struct limine_framebuffer *framebuffer = kernel.framebuffers[0];
    char    *labels[] = {"Width: ", "Height: ", "Pitch: ", "Bytes: "};
    uint64_t vals[]   = {framebuffer->width, framebuffer->height, framebuffer->pitch, kernel.framebuffer_size};
    uint64_t whole_len = 0;
    for (size_t i = 0; i < 4; i++)
        whole_len += ku_strlen(labels[i]) + 8; // 8 bytes for data
    char *temp_buffer = (char*) malloc(whole_len + 1);
    uint64_t upto = 0;
    for (uint64_t i = 0; i < 4; i++) {
        uint64_t this_strlen = ku_strlen(labels[i]);
        ku_memcpy((uint8_t*)(((uint64_t) temp_buffer) + upto), labels[i], this_strlen);
        ku_memcpy((uint8_t*)(((uint64_t)temp_buffer) + upto + this_strlen), ((char*) &vals[i]), 8);
        upto += this_strlen + 8;
    }
    size_t copy_amount = (max_len > upto) ? upto : max_len;
    ku_memcpy(buffer, temp_buffer, copy_amount);
    free(temp_buffer);
    return 0;
}

int write_fb_device(void *filev, char *buffer, size_t len) {
    struct limine_framebuffer *framebuffer = kernel.framebuffers[0];
    size_t copy_amount = (len > kernel.framebuffer_size) ? kernel.framebuffer_size : len;
    ku_memcpy((uint8_t*) kernel.back_buffer, buffer, copy_amount);
    swap_framebuffers();
    return 0;
}

int read_fb_device(void *filev, char *buffer, size_t max_len) {
    struct limine_framebuffer *framebuffer = kernel.framebuffers[0];
    size_t copy_amount = (max_len > kernel.framebuffer_size) ? kernel.framebuffer_size : max_len;
    ku_memcpy(buffer, (uint8_t*) kernel.back_buffer, copy_amount);
    swap_framebuffers();
    return 0;
}

void init_framebuffer() {
    kstatusf("Initiating framebuffers...");
    struct limine_framebuffer *framebuffer = kernel.framebuffers[0];
    kernel.framebuffer_size = framebuffer->width * (framebuffer->bpp / 8) * framebuffer->height;
    kernel.back_buffer = kmalloc(PAGE_ALIGN_UP(kernel.framebuffer_size) / 4096) + kernel.hhdm;
    ku_memset((uint8_t*) kernel.back_buffer, 0, kernel.framebuffer_size);
    DeviceOps fb_dev = (DeviceOps) {
        .read = &read_fb_device,
        .write = &write_fb_device,
        .open = &open_fb_device,
        .close = &close_fb_device
    };
    DeviceOps fb_info_dev = (DeviceOps) {
        .read = &read_fbinfo_device,
        .open = &open_fbinfo_device,
        .close = &close_fbinfo_device
    };
    create_device("D:/fb0", fb_dev);
    create_device("D:/fbinfo0", fb_info_dev);
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