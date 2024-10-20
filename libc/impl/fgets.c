#include <stdio.h>

char* fgets(char *str, int n, int fd) {
    read(fd, str, n);
    return str;
}
