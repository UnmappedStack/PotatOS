#include <stdio.h>
#include <event.h>

const char *argvals[] = {"R:/ramdiskroot/testuser", "Arg test 1!", "Second :D"};

int main(int argc, char **argv) {
    fputs("This message was printed from userspace! Trying to spawn process...\n", stdout);
    return 0;
}