[BITS 64]

section .text
    global _start

_start:
    mov rbx, 5
    push rbx
    int 0x80
    pop rax
    jmp $