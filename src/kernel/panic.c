#include "../utils/include/cpu_utils.h"
#include "kernel.h"
#include "../drivers/include/framebuffer.h"
#include "../utils/include/printf.h"
#include "../utils/include/string.h"
#include <stdint.h>

struct IDTEFrame {
    uint64_t cr2;
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rbp;
    uint64_t rdi;
    uint64_t rsi;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rbx;
    uint64_t rax;
    uint64_t type;
    uint64_t code;
    uint64_t rip;
    uint64_t cs;
    uint64_t flags;
    uint64_t rsp;
    uint64_t ss;
};

struct stackFrame {
    struct stackFrame* rbp;
    uint64_t rip;
};

void stack_trace(uint64_t rbp, uint64_t rip) {
    printf("\nStack Trace (Most recent call last): \n");
    printf(" 0x%x\n", rip);
    struct stackFrame *stack = (struct stackFrame*) rbp;
    while (stack) {
        printf(" 0x%x\n", stack->rip);
        stack = stack->rbp;
    }
}

void register_dump(struct IDTEFrame registers) {
    uint64_t idtr;
    uint64_t gdtr;
    uint64_t cr2;
    uint64_t cr3;
    asm("sgdt %0" : "=m"(gdtr));
    asm("sidt %0" : "=m"(idtr));
    asm("mov %%cr3, %0" : "=r"(cr3));
    asm("mov %%cr2, %0" : "=r"(cr2));
    printf("\nRegisters: \n");
    // gen purpose
    printf("  r8: 0x%x     r9: 0x%x     rax: 0x%x\n"
           " r10: 0x%x    r11: 0x%x     rbx: 0x%x\n"
           " r12: 0x%x    r13: 0x%x     rcx: 0x%x\n"
           " r14: 0x%x    r15: 0x%x     rdx: 0x%x\n",
           registers.r8, registers.r9, registers.rax, registers.r10, registers.r11, registers.rbx, registers.r12, registers.r13, registers.rcx, registers.r14, registers.r15, registers.rdx);
    // specifics
    printf("GDTR: 0x%x   IDTR: 0x%x\n"
           " CR3: 0x%x    CR2: 0x%x\n",
            gdtr, idtr, cr3, cr2);
}

void kpanic(char* message, struct IDTEFrame registers) {
    kernel.bg_colour = 0xFFFF00;
    kernel.fg_colour = 0xFF0000;
    printf("\n[KPANIC] The potato has gone rotten!\n");
    kernel.fg_colour = 0xd8d9d7;
    kernel.bg_colour = 0x012456;
    printf("Exception:  %s\n"
           "Error code: 0b%b\n",
            message, registers.ss);
    register_dump(registers);
    stack_trace(registers.rbp, registers.rip);
    printf("\n");
    halt();
}

void exception_handler(struct IDTEFrame registers) {
    if (kernel.in_exception_handler) halt();
    kernel.in_exception_handler = true;
    static char label_designate[30];
    char *label = (char*) label_designate;
    if (registers.type == 14)
        label = "Page Fault";
    else if (registers.type == 13)
        label = "General Protection Fault";
    else
        ku_uint16_to_string(registers.type, label);
    label_designate[29] = 0;
    kpanic(label, registers);
}


