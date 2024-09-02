#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "../drivers/include/serial.h"

void _start() {
    init_serial();
    write_serial("\nThis is PotatOS, the start of a new kernel to replace SpecOS and be far, far better!\nIt also has new lines :D\n");
    for(;;);
}
