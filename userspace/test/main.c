#include <stdio.h>
#include <event.h>

const char *argvals[] = {"R:/exec/shell", "Arg test 1!", "Second :D"};

int main(int argc, char **argv) {
    fputs("PotatOS PowerlessShell, compiled on " __DATE__ "\n"
          "This is free & open source software under the Mozilla Public License 2.0. See LICENSE in the GitHub repository for more information.\n", stdout);
    static char cwd_buffer[100];
    static char input_buffer[200];
    while (true) {
        get_cwd(cwd_buffer, 99);
        printf("\nSH %s> ", cwd_buffer);
        fgets(input_buffer, 200, stdin);
        printf("\n");
        int status = spawn(input_buffer, argvals, 3);
        for (;;);
    }
    for (;;);
    return 0;
}