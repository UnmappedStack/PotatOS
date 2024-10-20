#include <unistd.h>

size_t read(int fd, void *buf, size_t count) {
    syscall3(SYS_READ, fd, (uint64_t) buf, count);
    return count;
}
