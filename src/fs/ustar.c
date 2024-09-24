#include "include/ustar.h"
#include "../mem/include/kheap.h"
#include "../kernel/kernel.h"
#include "../fs/include/tempfs.h"
#include "../utils/include/printf.h"
#include "../utils/include/string.h"
#include "include/vfs.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

void unpack_ustar(char drive_char, char *archive_addr) {
    while (ku_memcmp(archive_addr + 257, "ustar", 5)) {
        uint64_t filesize = oct2bin(archive_addr + 0x7C, 11);
        uint8_t type = *((uint8_t*)(archive_addr + 156));
        if (type != '5') { // not a directory
            uint64_t len = ku_strlen(archive_addr) + 4;
            char *new_name = (char*) malloc(len);
            ku_memset((uint8_t*) new_name, 0, len);
            new_name[0] = drive_char;
            new_name[1] = ':';
            new_name[2] = '/';
            ku_memcpy(new_name + 3, archive_addr, ku_strlen(archive_addr));
            new_name[ku_strlen(archive_addr) + 4] = 0; // just to be safe
            File *f = open(new_name, O_CREATALL, MODE_WRITEONLY);
            if (f == NULL) {
                kfailf("Failed to create file (in unpack_ustar). Halting device, as this is a vital step in the boot process.\n");
                asm("cli; hlt");
            }
            int write_status = write(f, archive_addr + 512, filesize);
            if (write_status != 0) {
                kfailf("Failed to write to file \"%s\" (in unpack_ustar). Halting device.\n", new_name);
                asm("cli; hlt");
            }
            close(f);
            free(new_name);
        }
        archive_addr += (((filesize + 511) / 512) + 1) * 512;
    }
}

void setup_initrd() {
    kstatusf("Creating new TempFS...");
    Inode *new_tempfs = tempfs_new();
    k_ok();
    kstatusf("Mounting TempFS onto VFS drive `R:/`...");
    mount('R', FS_TEMPFS, true, (uintptr_t) new_tempfs, 0, 0);
    k_ok();
    kstatusf("Unpacking initial ramdisk onto the TempFS...");
    unpack_ustar('R', (char*) kernel.initial_ramdisk->address);
    k_ok();
}