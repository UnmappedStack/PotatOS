[BITS 64]

global pit_isr

extern task_select
extern task_switch_page_tree
extern task_get_entry_point
extern task_get_flags

pit_isr:
    jmp task_switch

task_switch:
    ;; push the registers of the current context
    push rax
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
    call task_select ; pushes next task addr onto the stack
    call task_switch_page_tree ; switch the address space based on the task selected (which is still on the stack)
    call task_get_flags ; get the flags of the task
    pop rbx ; Pop the Task* back from the stack (so that the flags can be accessed)
    pop rax ; Pop the task flags from the stack and store it in rax
    push rbx ; push Task* back onto the stack so that it can be used
    and rax, 0b10 ; check if it's the first exec
    jnz task_switch_first_exec ; if it is the first exec, jump to task_switch_first_exec
    jmp task_switch_previously_executed ; otherwise jump to task_switch_previously_executed

task_switch_first_exec:
    ;; clear all the gen purpose registers
    xor rax, rax
    xor rcx, rcx
    xor rdx, rdx
    xor rsi, rsi
    xor rdi, rdi
    xor rbp, rbp
    xor r8, r8
    xor r9, r9
    xor r10, r10
    xor r11, r11
    xor r12, r12
    xor r13, r13
    xor r14, r14
    xor r15, r15
    ;; push the interrupt stack
    call task_get_entry_point ; Task* is already on the stack
    pop rbx      ; pop Task* back off the stack so the entry point can be accessed
    xor rbx, rbx ; clear rbx as it's not needed
    ; rip is already on the stack from tasks_get_entry_point :)
    ; cs = 0x20 | 3
    mov rbx, 0x20
    or rbx, 3
    push rbx
    xor rbx, rbx
    ; rflags = 0
    push 0
    ; rsp = 0xFFFFFFFFFFFFF000
    push 0xFFFFFFFFFFFFF000
    ; ss = 0x28  | 3
    mov rbx, 0x28
    or rbx, 3
    push rbx
    xor rbx, rbx
    ;; jump to the entry point
    iretq

;; I wasn't sure what to name this label so it's kinda dumbly named lmao
task_switch_previously_executed:
    pop r15 ; pop Task* from the stack as it isn't needed
    xor r15, r15
    ;; pop general purpose registers from the stack, leaving only the interrupt frame registers
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
    pop rax
    ;; jump to the entry point
    iretq
