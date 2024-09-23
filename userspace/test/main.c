#include <stdio.h>

void main() {
    fputs("This message was printed from userspace!\n", stdout);
    for (;;);
}