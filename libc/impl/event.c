#include <event.h>

int poll(Event *buffer) {
    syscall1(SYS_POLL, (uint64_t) buffer);
}

int peek(Event *buffer) {
    syscall1(SYS_PEEK, (uint64_t) buffer);
}
