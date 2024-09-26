#include <stdio.h>
#include <event.h>

int main(int argc, char **argv) {
    fputs("This message was printed from userspace! Trying to do argument dump...\n", stdout);
    printf("Number of arguments: %i\n", argc);
    for (int i = 0; i < argc; i++)
        printf("Argument %i: %s\n", i, argv[i]);
    return 0;
}