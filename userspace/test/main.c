#include <stdio.h>
#include <event.h>

int main(int argc, char **argv) {
    fputs("This message was printed from userspace!\n", stdout);
    return 0;
}