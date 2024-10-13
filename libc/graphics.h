#pragma once
#include <stdio.h>

typedef struct {
    File buffer;
    File info;
    uint64_t height;
    uint64_t width;
    uint64_t pitch;
    uint64_t bytes;
} Framebuffer;

char fbinfo[100];

Framebuffer fb = {0};

Framebuffer open_framebuffer();

#ifndef OPEN_FB_IMPL
Framebuffer open_framebuffer() {
    fb.info   = open("D:/fbinfo0", 0, MODE_READONLY);
    fb.buffer = open("D:/fb0", 0, MODE_READWRITE);
    fgets(fbinfo, 99, fb.info);
    fb.width  = *((uint64_t*)(fbinfo +  7));
    fb.height = *((uint64_t*)(fbinfo + 23));
    fb.pitch  = *((uint64_t*)(fbinfo + 38));
    fb.bytes  = *((uint64_t*)(fbinfo + 53));
    return fb;
}
#endif

void close_framebuffer();

#ifndef CLOSE_FB_IMPL
void close_framebuffer() {
    close(fb.info);
    close(fb.buffer);
}
#endif