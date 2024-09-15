#include "include/ustar.h"
#include "../mem/include/kheap.h"
#include "../utils/include/printf.h"
#include "../utils/include/string.h"
#include "include/vfs.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

void unpack_ustar(char drive_char, char *archive_addr) {
    printf("\n");
    while (ku_memcmp(archive_addr + 257, "ustar", 5)) {
        uint64_t filesize = oct2bin(archive_addr + 0x7C, 11);
        uint8_t type = *((uint8_t*)(archive_addr + 156));
        if (type != '5') { // not a directory
            char *new_name = (char*) malloc(ku_strlen(archive_addr) + 3);
            new_name[0] = drive_char;
            new_name[1] = ':';
            new_name[2] = '/';
            ku_memcpy(new_name + 3, archive_addr, ku_strlen(archive_addr));
            kdebugf("Create path: %s\n", new_name);
            File *f = open(new_name, O_CREATALL);
            close(f);
            free(new_name);
        }
        archive_addr += (((filesize + 511) / 512) + 1) * 512;
    }
}