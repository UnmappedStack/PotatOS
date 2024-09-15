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
    struct stackFrame *stack = (struct stackFrame*)rbp;
    while (stack && stack->rip) {
        printf(" 0x%x\n", stack->rip);
        stack = stack->rbp;
    }
}

void register_dump() {
    uint64_t idtr;
    uint64_t gdtr;
    uint64_t cr2;
    uint64_t cr3;
    asm("sgdt %0" : "=m"(gdtr));
    asm("sidt %0" : "=m"(idtr));
    asm("mov %%cr3, %0" : "=r"(cr3));
    asm("mov %%cr2, %0" : "=r"(cr2));
    printf("\nRegisters: \n");
    printf("GDTR: 0x%x\nIDTR: 0x%x\n"
           " CR3: 0x%x\n CR2: 0x%x\n",
            gdtr, idtr, cr3, cr2);
}

void kpanic(char* message, struct IDTEFrame registers) {
    fill_screen(0xFF0000);
    printf(BRED "\n[KPANIC] " WHT "The potato has gone rotten!\n");
    printf("Exception:  %s\n"
           "Error code: 0b%b\n",
            message, registers.ss);
    register_dump();
    stack_trace(registers.rbp, registers.rip);
    printf("\n");
    asm("cli; hlt");
}

void exception_handler(struct IDTEFrame registers) {
    if (kernel.in_exception_handler)
        asm("cli; hlt");
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


