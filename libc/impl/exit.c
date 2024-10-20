#include <stdlib.h>

void exit(int status) {
    syscall1(SYS_EXIT, status);
}
