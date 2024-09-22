#include <stddef.h>
#include <stdint.h>

size_t strlen(const char* str) {
    size_t len = 0;
    while (str[len])
        len++;
    return len;
}

void write_screen(char *str) {
    size_t len = strlen(str);
    asm volatile (
        "movq $1, %%rdi\n"
        "movq %0, %%rsi\n"
        "movq %1, %%rdx\n"
        "movq $1, %%rax\n"
        "int $0x80"
        : : "r" ((uint64_t) str), "r" ((uint64_t) len)
        : "%rdi", "%rsi", "%rdx", "%rax"
    );
}

void _start() {
    char *msg = "Hello world from a proper userspace syscall (but written in C this time)!\n\n";
    write_screen(msg);
    for (;;);
}
