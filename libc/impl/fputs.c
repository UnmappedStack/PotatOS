#include <stdio.h>

int fputs(const char *str, int stream) {
    return write(stream, str, strlen(str));
}
