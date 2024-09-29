#include <stdio.h>
#include <event.h>

int main(int argc, char **argv) {
    fputs("This message was printed from userspace! You can now type with the keyboard.\n > ", stdout);
    static char buff[200];
    fgets(buff, 200, stdin);
    printf("\nYou typed: \"%s\"\n", buff);
    return 0;
}