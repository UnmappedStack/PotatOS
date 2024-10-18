[BITS 64]

global timer_isr

extern check_task_switch_allowed
extern pit_decrement_counter
extern end_of_interrupt
extern unlock_pit
extern unlock_lapic_timer
extern lock_lapic_timer
extern task_get_argc
extern task_get_argv
extern task_disable_first_exec
extern task_get_kernel_stack
extern task_get_rsp
extern printf
extern task_select
extern task_get_cr3
extern task_get_entry_point
extern task_get_flags
extern lock_pit
extern task_set_rsp
extern get_current_task

%define STACK_PAGES 2
%define PAGE_SIZE 4096
%define USER_STACK_PTR 0x700000000000
%define USER_STACK_ADDR (USER_STACK_PTR - STACK_PAGES * PAGE_SIZE)

%define KERNEL_STACK_PTR 0xFFFFFFFFFFFFF000
%define KERNEL_STACK_ADDR (KERNEL_STACK_PTR - STACK_PAGES * PAGE_SIZE)


timer_isr:
    jmp task_switch

task_switch:
    ;; push all the registers onto the stack
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
    call pit_decrement_counter
    call check_task_switch_allowed
    test rax, rax
    jz cannot_task_switch
    ; set this task's rsp to the current rsp
    call get_current_task
    mov rdi, rax
    mov rsi, rsp
    call task_set_rsp
    ; select new task etc
    call task_select ; gets next task and puts it into rax
    mov rdi, rax ; put the return val (Task*) into rdi to pass into task_switch_page_tree
    mov r15, rax ; keep Task* for later in r15.
    call task_get_cr3 ; get the pml4 based on the task selected (which is in rdi)
    mov cr3, rax ; switch the page tree
    mov rdi, r15 ; get Task* back from r15 and put it into rdi to pass into task_get_flags & task_get_rsp
    ; set the current rsp to that of the new task
    call task_get_rsp
    mov rsp, rax
    call task_get_flags ; get the flags of the task
    and rax, 0b10 ; check if it's the first exec
    jnz task_switch_first_exec ; if it is the first exec, jump to task_switch_first_exec
    jmp task_switch_previously_executed ; otherwise jump to task_switch_previously_executed
    jmp $

task_switch_first_exec:
    mov rdi, r15 ; get Task* back from r15 and put it into rdi to pass into task_get_entry_point
    ;; turn off first exec flag
    call task_disable_first_exec
    ; push errno (starts as 0)
    push 0
    ;; push the interrupt stack
    ; ss = 0x20 | 3
    mov rbx, 0x20
    or rbx, 3
    push rbx
    xor rbx, rbx
    ; rsp = 0x6FFFFFFFFFF8
    mov rbx, 0x6FFFFFFFFFF8
    push rbx
    ; rflags = 0x200
    push 0x200
    ; cs = 0x18 | 3
    mov rbx, 0x18
    or rbx, 3
    push rbx
    xor rbx, rbx
    ; rip
    call task_get_entry_point
    push rax
    call unlock_lapic_timer
    ;; pass it the cmd line args
    call end_of_interrupt
    call task_get_argc
    mov r15, rax
    call task_get_argv
    mov rdi, r15
    mov rsi, rax
    ;; clear all the gen purpose registers
    xor rax, rax
    xor rbx, rbx
    xor rcx, rcx
    xor rdx, rdx
    xor rbp, rbp
    xor r8, r8
    xor r9, r9
    xor r10, r10
    xor r11, r11
    xor r12, r12
    xor r13, r13
    xor r14, r14
    xor r15, r15
    iretq
    jmp $

;; I wasn't sure what to name this label so it's kinda dumbly named lmao
task_switch_previously_executed:
    call end_of_interrupt
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
    jmp $

cannot_task_switch:
    call end_of_interrupt
    ; just pop all registers and return
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
    iretq

print_debug_info:
    mov rdi, msg
    pop rsi
    pop rdx
    pop rcx
    pop r8
    pop r9
    ;; it still needs to be on the stack
    push r9
    push r8
    push rcx
    push rdx
    push rsi
    call printf
    iretq
    ret

msg:  db 10, "rip: 0x%x, cs: %i, rflags: %i, rsp: 0x%x, ss: %i", 10, 0
msg1: db 10, "rsp: 0x%x", 10, 0
