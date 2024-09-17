#include <stddef.h>
#include "../utils/include/printf.h"
#include "include/spawn.h"
#include "../fs/include/vfs.h"

int spawn(char *path) {
    char buffer[5];
    File *f = open(path, 0, MODE_READONLY);
    if (f == NULL) {
        return 2;
    }
    int status = read(f, buffer, 4);
    if (status != 0) {
        close(f);
        return 1;
    }
    close(f);
    buffer[4] = 0;
    kdebugf("First 4 chars of ELF: %s", buffer);
    return 0;
}