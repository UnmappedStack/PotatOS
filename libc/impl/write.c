#include <unistd.h>

size_t write(int fd, const void *buf, size_t count) {
    return syscall3(SYS_WRITE, fd, (uint64_t) buf, count);
}
