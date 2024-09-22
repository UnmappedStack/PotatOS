;; The design of how I handle syscalls is inspired by Dcraftbg a bit :D
[BITS 64]

PTR_SIZE equ 8

extern lock_pit
extern unlock_pit

extern syscall_msg1
extern syscall_msg2
extern syscall_msg3
extern syscall_invalid

syscall_lookup:
    dq syscall_msg1
    dq syscall_msg2
    dq syscall_msg3
syscall_lookup_end:

global syscall_isr

syscall_isr:
    cmp rax, (syscall_lookup_end-syscall_lookup) / 8
    jge invalid_syscall
    push rax
    call lock_pit
    pop rax
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
    call unlock_pit
    ret

invalid_syscall:
    call syscall_invalid
    ret