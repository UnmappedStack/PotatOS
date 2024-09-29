#pragma once
#include <stdint.h>
#include <stddef.h>

void init_devices();

typedef struct {
    int (*read )(void *file, char *buffer, size_t max_len);
    int (*write)(void *file, char *buffer, size_t len);
    void (*open)(void *file, uint8_t mode);
    void (*close)(void *file);
} DeviceOps;
