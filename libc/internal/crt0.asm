[BITS 64]

extern main
global _start

_start:
    ;; Push argc and argv values
    push rdi
    push rsi
    ;; Initiate anything needed
    call init_libc
    ;; Restore argc and argv values
    pop rsi
    pop rdi
    ;; Call main()
    call main
    ;; Return with status code
    mov rdi, rax ; Move main() return value into first input of exit syscall
    mov rax, 9 ; Syscall number for exit syscall
    int 0x80 ; Hey kernel! Exit plz

init_libc:
    ;; this is just a stub for now
    ret
