#include "include/syscalls.h"
#include "../utils/include/printf.h"
#include "../kernel/kernel.h"
#include "../cpu/include/idt.h"
#include "../drivers/include/pit.h"

void syscall_msg1() {
    printf("Message from syscall #1 :D\n");
}

void syscall_msg2() {
    printf("Message from syscall #2 :(\n");
}

void syscall_msg3() {
    printf("Message from syscall #3 O_O\n");
}

void syscall_invalid() {
    printf("Hey that syscall doesn't exist! (Invalid syscall!)\n");
}

extern void syscall_isr();

void init_syscalls() {
    kstatusf("Initiating syscalls...");
    struct IDTEntry *IDT = (struct IDTEntry*) kernel.idtr.offset;
    set_IDT_entry(0x80, &syscall_isr, 0xEF, IDT);
    printf(BGRN " Ok!\n" WHT);
}