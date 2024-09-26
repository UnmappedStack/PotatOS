#include <stdio.h>
#include <event.h>

int main(int argc, char **argv) {
    fputs("This message was printed from userspace! Trying to do argument dump...\n", stdout);
    printf("Number of arguments: %i\n", argc);
    for (int i = 0; i < argc; i++)
        printf("Argument %i: %s\n", i, argv[i]);
    fputs("Trying to open R:/ramdiskroot/testfile1.txt...\n", stdout);
    File f = open("R:/ramdiskroot/testfile1.txt", 0, MODE_READONLY);
    printf("Success! File descriptor = %i\n", f);
    printf("Trying to close...\n");
    close(f);
    printf("Closed successfully!\n");
    return 0;
}