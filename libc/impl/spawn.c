#include <unistd.h>

uint64_t spawn(char *path, const char **argv, uint64_t argc) {
    return syscall3(SYS_SPAWN, (uint64_t) path, (uint64_t) argv, argc);
}
