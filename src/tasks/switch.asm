[BITS 64]

global pit_isr

extern printf
extern task_select
extern task_get_cr3
extern task_get_entry_point
extern task_get_flags
extern lock_pit

pit_isr:
    jmp task_switch

;switch_gdt_selectors:
;    mov ds, 0x2b
;    mov es, 0x2b
;    mov fs, 0x2b
;    mov gs, 0x2b
;    mov ss, 0x2b
;    ret

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
    call task_select ; gets next task and puts it into rax
    mov rdi, rax ; put the return val (Task*) into rdi to pass into task_switch_page_tree
    mov r15, rax     ; keep Task* for later in r15.
    call task_get_cr3 ; get the pml4 based on the task selected (which is in rdi)
    mov cr3, rax ; switch the page tree
    mov rdi, r15 ; get Task* back from r15 and put it into rdi to pass into task_get_flags
    call task_get_flags ; get the flags of the task
    and rax, 0b10 ; check if it's the first exec
    jnz task_switch_first_exec ; if it is the first exec, jump to task_switch_first_exec
    jmp task_switch_previously_executed ; otherwise jump to task_switch_previously_executed

task_switch_first_exec:
    ;; push the interrupt stack
    mov rdi, r15 ; get Task* back from r15 and put it into rdi to pass into task_get_entry_point
    ; ss = 0x28 | 3
    mov rbx, 0x28
    or rbx, 3
    push rbx
    xor rbx, rbx
    ; rsp = 0x700000000000
    mov rbx, 0x700000000000
    push rbx
    ; rflags = 0b10
    push 0b10
    ; cs = 0x20 | 3
    mov rbx, 0x20
    or rbx, 3
    push rbx
    xor rbx, rbx
    ; rip
    call task_get_entry_point
    xor r15, r15 ; <-|-- r15 and rdi are no longer needed, clear them
    xor rdi, rdi ; <-|
    push rax
    xor rax, rax ; rax is no longer needed 
    ;; clear all the gen purpose registers
    xor rax, rax
    xor rbx, rbx
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
    mov rdi, msg
    pop rsi
    pop rdx
    pop rcx
    pop r8
    pop r9
    call printf
;    call switch_gdt_selectors
    iretq
    jmp $
    
msg: db "Testing stack. rip = 0x%x, cs = %i, rflags = %i, rsp = 0x%x, ss = %i", 10, 0

;; I wasn't sure what to name this label so it's kinda dumbly named lmao
task_switch_previously_executed:
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
