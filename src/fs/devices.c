#include <stdint.h>
#include <stddef.h>
#include "../utils/include/string.h"
#include "include/devices.h"
#include "include/tempfs.h"
#include "include/vfs.h"
#include "../utils/include/printf.h"

// some simple devices
void devzero_open(void *file, uint8_t mode) {
    if (mode != MODE_READONLY)
        kfailf("The devzero special file (D:/devzero) can only be opened as MODE_READONLY, but it was opened with another mode. Failed to open device.\n");
}

void devzero_close(void *file) {};

int devzero_read(void *file, char* buffer, size_t maxlen) {
    ku_memset(buffer, 0, maxlen);
}

void init_devices() {
    kstatusf("Initiating devices...");
    Inode *devfs = tempfs_new();
    mount('D', FS_TEMPFS, true, (uintptr_t) devfs, 0, 0);
    // initiate some of the basic devices
    DeviceOps devzero = (DeviceOps) {
        .read  = devzero_read,
        .open  = devzero_open,
        .close = devzero_close
    };
    create_device("D:/devzero", devzero);
    k_ok();
}
