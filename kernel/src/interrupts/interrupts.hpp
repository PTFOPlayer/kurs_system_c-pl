#pragma once
#include <stdint.h>

#include "../graphics/graphics.hpp"
#include "../utils/utils.hpp"
#include "interrupts_msg.hpp"

#define MASTER_PIC 0x20
#define MASTER_PIC_DATA 0x21
#define SLAVE_PIC 0xA0
#define SLAVE_PIC_DATA 0xA1

struct IDTEntry {
    uint16_t isr_low;
    uint16_t kernel_cs;
    uint8_t ist;
    uint8_t attributes;
    uint16_t isr_mid;
    uint32_t isr_high;
    uint32_t reserved;
} __attribute__((packed));

__attribute__((aligned(0x10))) static IDTEntry idt[256];

struct IDTR {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

static IDTR idtr;

struct ExceptionFrame {
    uint64_t error_code;
    uint64_t flags;
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8, rdi, rsi, rsp, rbp, rdx, rcx,
        rbx, rax;
};

const uint8_t dbg_registers_cond = 1;

void dbg_register_pair(char* reg1, char* reg2, uint64_t var1, uint64_t var2) {
    printf("%s: 0x%x\t\t%s: 0x%x\n", reg1, var1, reg2, var2);
}

extern "C" void exception_handler(struct ExceptionFrame* frame) {
    char* msg = "unknown";
    if (frame->error_code < 32) {
        msg = interrupts_msg[frame->error_code];
    }

    if (text_mode) {
        error("\n\nInterrupt occured\n%s\n", msg);
    }

    if (text_mode && dbg_registers_cond) {
        printf("flags: 0x%x\n", frame->flags);
        dbg_register_pair("rax", "rbx", frame->rax, frame->rbx);
        dbg_register_pair("rcx", "rdx", frame->rcx, frame->rdx);
        dbg_register_pair("rbp", "rsp", frame->rbp, frame->rsp);
        dbg_register_pair("rsi", "rdi", frame->rsi, frame->rdi);
        dbg_register_pair("r8 ", "r9 ", frame->r8, frame->r9);
        dbg_register_pair("r10", "r11", frame->r10, frame->r11);
        dbg_register_pair("r12", "r13", frame->r12, frame->r13);
        dbg_register_pair("r14", "r15", frame->r14, frame->r15);
    }

    asm volatile("cli");
    while (1) asm volatile("hlt");

    return;
}

void idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags) {
    IDTEntry* descriptor = &idt[vector];

    descriptor->isr_low = (uint64_t)isr & 0xFFFF;
    descriptor->kernel_cs = 0x28;
    descriptor->ist = 0;
    descriptor->attributes = flags;
    descriptor->isr_mid = ((uint64_t)isr >> 16) & 0xFFFF;
    descriptor->isr_high = ((uint64_t)isr >> 32) & 0xFFFFFFFF;
    descriptor->reserved = 0;
}

#define IDT_MAX_DESCRIPTORS 255
static bool vectors[IDT_MAX_DESCRIPTORS];

extern void* isr_stub_table[];
extern void* irq_stub_table[];

typedef struct IRQFrame {
    uint64_t idx;
    uint64_t flags;
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8, rdi, rsi, rsp, rbp, rdx, rcx,
        rbx, rax;
} IRQFrame;

void (*irq_routines[16])(IRQFrame frame) = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

extern "C" void irq_handler(IRQFrame* frame) {
    void (*routine)(IRQFrame frame) = irq_routines[frame->idx];
    if (routine) {
        routine(*frame);
    }

    if (frame->idx >= 8) {
        out_b(SLAVE_PIC, 0x20);
    }

    out_b(MASTER_PIC, 0x20);
}

void irq_install(uint64_t irq, void (*handler)(IRQFrame frame)) {
    irq_routines[irq] = handler;
}

void irq_remove(uint64_t irq) { irq_routines[irq] = 0; }

void idt_init() {
    idtr.base = (uintptr_t)&idt[0];
    idtr.limit = (uint16_t)sizeof(IDTEntry) * IDT_MAX_DESCRIPTORS - 1;

    for (uint8_t vector = 0; vector < 48; vector++) {
        if (vector < 32) {
            idt_set_descriptor(vector, isr_stub_table[vector], 0x8E);
            vectors[vector] = 1;
        } else {
            idt_set_descriptor(vector, irq_stub_table[vector - 32], 0x8E);
            vectors[vector] = 1;
        }
    }

    out_b(MASTER_PIC, 0x11);
    out_b(SLAVE_PIC, 0x11);

    out_b(MASTER_PIC_DATA, 0x20);
    out_b(SLAVE_PIC_DATA, 0x28);

    out_b(MASTER_PIC_DATA, 0x04);
    out_b(SLAVE_PIC_DATA, 0x02);

    out_b(MASTER_PIC_DATA, 0x01);
    out_b(SLAVE_PIC_DATA, 0x01);

    out_b(MASTER_PIC_DATA, 0x0);
    out_b(SLAVE_PIC_DATA, 0x0);

    __asm__ volatile("lidt %0" : : "m"(idtr));
    __asm__ volatile("sti");

    info("IDT initialized \n");
}
