[BITS 64]

section .text
    global _start

_start:
    mov rdi, 1
    mov rsi, msg
    mov rdx, len
    mov rax, 0
    int 0x80
    jmp $

msg: db "Hello world from a proper userspace syscall!", 10, 10, 0
len equ $ - msg