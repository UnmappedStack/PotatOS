[BITS 64]

section .text
    global _start

_start:
    mov rdi, 1    ; stdout file selector
    mov rsi, msg  ; give it the message
    mov rdx, len  ; give it the length
    mov rax, 1    ; write() syscall
    int 0x80
    jmp $

msg: db "Hello world from a proper userspace syscall!", 10, 10, 0
len equ $ - msg