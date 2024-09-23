#include "include/devices.h"
#include "include/tempfs.h"
#include "include/vfs.h"
#include "../utils/include/printf.h"

void init_devices() {
    kstatusf("Initiating devices...");
    Inode *devfs = tempfs_new();
    mount('D', FS_TEMPFS, true, (uintptr_t) devfs, 0, 0);
    printf(" Ok!\n");
}
