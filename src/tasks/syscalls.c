#include "include/syscalls.h"
#include "../tasks/include/tasklist.h"
#include "../utils/include/printf.h"
#include "../kernel/kernel.h"
#include "../cpu/include/idt.h"
#include "../drivers/include/pit.h"
#include "../fs/include/vfs.h"

/* Write syscall handler
 * rdi = File descriptor
 * rsi = Buffer address
 * rdx = Buffer length
 */
int syscall_write(uint64_t rdi, uint64_t rsi, uint64_t rdx) {
    Task current_task = get_task(kernel.tasklist.current_task); 
    File *f = current_task.resources[rdi];
    if (!f->present) {
        printf("File could not be written to, has not been opened.\n");
        return 1;
    }
    int write_status = write(f, (char*) rsi, rdx);
    if (write_status != 0) return write_status;
    return 0;
}

/* Read syscall handler
 * rdi = File descriptor
 * rsi = Buffer
 * rdx = length
 */
int syscall_read(uint64_t rdi, uint64_t rsi, uint64_t rdx) {
    Task current_task = get_task(kernel.tasklist.current_task); 
    File *f = current_task.resources[rdi];
    if (!f->present) {
        printf("File could not be read from, has not been opened.\n");
        return 1;
    }
    int read_status = read(f, (char*) rsi, rdx);
    if (read_status != 0) return read_status;
    return 0;
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