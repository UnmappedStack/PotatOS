#include <unistd.h>

char* getcwd(char *buf, size_t size) {
    syscall2(SYS_GETCWD, (uint64_t) buf, size);
    return buf;
}
