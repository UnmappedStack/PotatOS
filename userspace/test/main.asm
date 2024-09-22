[BITS 64]

section .text
    global _start

_start:
    mov rax, 0
    int 0x80
    mov rax, 1
    int 0x80
    mov rax, 2
    int 0x80
    mov rax, 3
    int 0x80
    jmp $