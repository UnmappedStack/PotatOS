#include <string.h>

size_t strlen(const char *str) {
    size_t len = 0;
    while (str[len]) len++;
    return len;
}

void* memcpy(void* dest, const void* from, size_t n) {
    asm ("rep movsb" : : "D" (dest), "S" (from), "c" (n) : "memory");
    return dest;
}

int memcmp(const void *str1, const void *str2, size_t size) {
    for (size_t c = 0; c < size; c++) {
        if (((uint8_t*) str1)[c] != ((uint8_t*) str2)[c]) return (*((uint8_t*)str1) - *((uint8_t*)str2));
    }
    return 0;
}
