#include "../utils/include/string.h"
#include "include/spawn.h"
#include "include/syscalls.h"
#include "../mem/include/kheap.h"
#include "include/events.h"
#include "../tasks/include/tasklist.h"
#include "../utils/include/printf.h"
#include "../kernel/kernel.h"
#include "../cpu/include/idt.h"
#include "../drivers/include/pit.h"
#include "../fs/include/vfs.h"
#include "include/end_task.h"

/* Write syscall handler
 * rdi = File descriptor
 * rsi = Buffer address
 * rdx = Buffer length
 */
int syscall_write(uint64_t rdi, uint64_t rsi, uint64_t rdx) {
    Task *current_task = get_task(kernel.tasklist.current_task); 
    File *f = current_task->resources[rdi];
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
    Task *current_task = get_task(kernel.tasklist.current_task); 
    File *f = current_task->resources[rdi];
    if (!f->present) {
        printf("File could not be read from, has not been opened.\n");
        return 1;
    }
    int read_status = read(f, (char*) rsi, rdx);
    if (read_status != 0) return read_status;
    return 0;
}

/* get_cwd syscall handler
 * rdi = buffer
 * rsi = buffer length
 */
void syscall_get_cwd(uint64_t rdi, uint64_t rsi) {
    Task *current_task = get_task(kernel.tasklist.current_task);
    uint64_t cd_len   = ku_strlen(current_task->current_dir);
    uint64_t copy_len = (rsi > cd_len) ? cd_len : rsi;
    ku_memcpy((char*) rdi, current_task->current_dir, copy_len);
}

/* Get event from event queue
 * rdi = event buffer.
 */
void syscall_get_event(uint64_t rdi) {
    Task  *current_task = get_task(kernel.tasklist.current_task);
    Event *this_event   = get_event(current_task->event_queue);
    *((Event*) rdi)     = *this_event;
    free(this_event);
}

/* Open a file
 * rdi = filename buffer
 * rsi = flags
 * rdx = mode
 *
 * returns file descriptor number in rax
 */
uint64_t syscall_open(uint64_t rdi, uint64_t rsi, uint64_t rdx) {
    uint64_t file_descriptor = 0;
    uint64_t current_task_id = kernel.tasklist.current_task;
    Task *current_task = get_task(current_task_id);
    for (; file_descriptor < NUM_RESOURCES; file_descriptor++) {
        if (current_task->resources[file_descriptor] != 0) continue;
        current_task->resources[file_descriptor] = open((char*) rdi, rsi, rdx);
        return file_descriptor;
    }
    kfailf("Couldn't open new file: too many files already open. Exiting program.\n");
    try_exit_task(1);
    return 1;
}

/* Close a file
 * rdi = file descriptor
 */

void syscall_close(uint64_t rdi) {
    Task *current_task = get_task(kernel.tasklist.current_task);
    close(current_task->resources[rdi]);
    current_task->resources[rdi] = NULL;
}

/* Spawn a new process & create an event when it's finished executing
 * rdi = program path buffer
 * rsi = argv (**char)
 * rdx = argc
 */

int syscall_spawn(uint64_t rdi, uint64_t rsi, uint64_t rdx) {
    return spawn((char*) rdi, (const char**) rsi, rdx);
}

void syscall_invalid() {
    printf("Hey that syscall doesn't exist! (Invalid syscall!)\n");
}

extern void syscall_isr();

void init_syscalls() {
    kstatusf("Initiating syscalls...");
    struct IDTEntry *IDT = (struct IDTEntry*) kernel.idtr.offset;
    set_IDT_entry(0x80, &syscall_isr, 0xEF, IDT);
    k_ok();
}