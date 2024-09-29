#include "include/keyboard.h"
#include "../kernel/kernel.h"
#include "../utils/include/printf.h"
#include "../utils/include/io.h"
#include "include/irq.h"
#include "../cpu/include/idt.h"
#include <stddef.h>
#include "../fs/include/devices.h"
#include "../fs/include/vfs.h"

#define PS2_DATA_REGISTER    0x60
#define PS2_STATUS_REGISTER  0x64
#define PS2_COMMAND_REGISTER 0x64

__attribute__((interrupt))
void keyboard_isr(void*) {
    printf("Key pressed: 0x%x\n", inb(PS2_DATA_REGISTER));
    end_of_interrupt();
}

int read_ps2_kb(void *file, char *buffer, size_t max_len) {
    kdebugf("Read keyboard recieved from device! :D\n");
    return 0;
}

void open_ps2_kb(void *file) {
    kdebugf("Keyboard device opened.\n");
}

void init_ps2_keyboard() {
    kstatusf("Initiating PS/2 keyboard...\n");
    DeviceOps kb_dev = (DeviceOps) {
        .read = &read_ps2_kb,
        .open = &open_ps2_kb
    };
    create_device("D:/stdin", kb_dev);
    set_IDT_entry(33, &keyboard_isr, 0x8E, (struct IDTEntry*) kernel.idtr.offset);
    unmask_irq(1);
}











