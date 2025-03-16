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
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8, rdi, rsi, rsp, rbp, rdx, rcx,
        rbx, rax;
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t optional_rsp;
    uint64_t optional_ss;
} __attribute__((packed));

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
        printf("CS: 0x%x\t| RIP: 0x%x \t| Flags: 0b%b\n", frame->cs, frame->rip,
               frame->rflags);
        printf("rax: 0x%x\t|\trbx: 0x%x\t|\trcx: 0x%x\t|\trdx: 0x%x\n",
               frame->rax, frame->rbx, frame->rcx, frame->rdx);
        printf("rbp: 0x%x\t|\trsp: 0x%x\t|\trsi: 0x%x\t|\trdi: 0x%x\n",
               frame->rbp, frame->rsp, frame->rsi, frame->rdi);
        printf("r8: 0x%x\t|\r9: 0x%x\t|\tr10: 0x%x\t|\r11: 0x%x\n", frame->r8,
               frame->r9, frame->r10, frame->r11);
        printf("r12: 0x%x\t|\r13: 0x%x\t|\tr14: 0x%x\t|\tr15: 0x%x\n",
               frame->r12, frame->r13, frame->r14, frame->r15);
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

struct IRQFrame {
    uint64_t idx;
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8, rdi, rsi, rsp, rbp, rdx, rcx,
        rbx, rax;
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t optional_rsp;
    uint64_t optional_ss;
} __attribute__((packed));

void (*irq_routines[16])(IRQFrame* frame) = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

extern "C" void irq_handler(IRQFrame* frame) {
    void (*routine)(IRQFrame* frame) = irq_routines[frame->idx];
    if (routine) {
        routine(frame);
    }

    if (frame->idx >= 8) {
        out_b(SLAVE_PIC, 0x20);
    }

    out_b(MASTER_PIC, 0x20);
}

void irq_install(uint64_t irq, void (*handler)(IRQFrame* frame)) {
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
