#include "include/keyboard.h"
#include "../kernel/kernel.h"
#include "../utils/include/printf.h"
#include "../utils/include/io.h"
#include "include/irq.h"
#include "../cpu/include/idt.h"

#define PS2_DATA_REGISTER    0x60
#define PS2_STATUS_REGISTER  0x64
#define PS2_COMMAND_REGISTER 0x64

__attribute__((interrupt))
void keyboard_isr(void*) {
    printf("Key pressed: 0x%x\n", inb(PS2_DATA_REGISTER));
    end_of_interrupt();
}

void init_ps2_keyboard() {
    kstatusf("Initiating PS/2 keyboard...");
    set_IDT_entry(33, &keyboard_isr, 0x8E, (struct IDTEntry*) kernel.idtr.offset);
    unmask_irq(1);
    k_ok();
}