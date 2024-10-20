;; The design of how I handle syscalls is inspired by Dcraftbg a bit :D
[BITS 64]

PTR_SIZE equ 8

extern lock_syscall_handler
extern unlock_syscall_handler
extern lock_lapic_timer
extern unlock_lapic_timer
extern syscall_mmap
extern syscall_exit
extern syscall_write
extern syscall_read
extern syscall_get_errno
extern syscall_poll
extern syscall_peek
extern syscall_invalid
extern syscall_open
extern syscall_close
extern syscall_spawn
extern syscall_get_cwd

syscall_lookup:
    dq syscall_read
    dq syscall_write
    dq syscall_open
    dq syscall_close
    dq syscall_spawn
    dq syscall_get_cwd
    dq syscall_poll
    dq syscall_peek
    dq syscall_get_errno
    dq syscall_exit
    dq syscall_mmap
syscall_lookup_end:

global syscall_isr

syscall_isr:
    cli
    cmp rax, (syscall_lookup_end-syscall_lookup) / 8
    jae invalid_syscall
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    call [syscall_lookup + rax * 8]
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    iretq

invalid_syscall:
    call syscall_invalid
    ret

msg1: db 10, "Doing syscall, rax = %i", 10, 0
