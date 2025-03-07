#pragma once
#include "../stdlib/stdint.h"
#include "../stdlib/utils.h"
#include "../stdlib/io.h"
#include "../vga/vga.h"
typedef struct {
    u16 isr_low;
    u16 kernel_cs;
    u8 ist;
    u8 attributes;
    u16 isr_mid;
    u32 isr_high;
    u32 reserved;
} __attribute__((packed)) idt_entry_t;

__attribute__((aligned(0x10))) static idt_entry_t idt[256];

typedef struct {
    u16 limit;
    u64 base;
} __attribute__((packed)) idtr_t;

static idtr_t idtr;

char* int_messages[] = {"Division By Zero",
                        "Debug",
                        "Non Maskable Interrupt",
                        "Breakpoint",
                        "Into Detected Overflow",
                        "Out of Bounds",
                        "Invalid Opcode",
                        "No Coprocessor",
                        "Double fault",
                        "Coprocessor Segment Overrun",
                        "Bad TSS",
                        "Segment not present",
                        "Stack fault",
                        "General protection fault",
                        "Page fault",
                        "Unknown Interrupt",
                        "Coprocessor Fault",
                        "Alignment Fault",
                        "Machine Check",
                        "Reserved",
                        "Reserved",
                        "Reserved",
                        "Reserved",
                        "Reserved",
                        "Reserved",
                        "Reserved",
                        "Reserved",
                        "Reserved",
                        "Reserved",
                        "Reserved",
                        "Reserved",
                        "Reserved"};

struct ExceptionFrame {
    u64 error_code;
    u64 flags;
    u64 r15, r14, r13, r12, r11, r10, r9, r8, rdi, rsi, rsp, rbp, rdx, rcx, rbx,
        rax;
};

const u8 dbg_registers_cond = 1;


void dbg_register_pair(char* reg1, char* reg2, u64 var1, u64 var2) {
    printf("%s: 0x%x\t\t%s: 0x%x\n", reg1, var1, reg2, var2);
}

void exception_handler(struct ExceptionFrame* frame) {
    set_color(Red, Yellow);
    char* msg = "unknown";
    if (frame->error_code < 32) {
        msg = int_messages[frame->error_code];
    }

    printf("\n\nInterrupt occured\n%s\n", msg);

    if (dbg_registers_cond) {
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
    while (1)
        asm volatile("hlt");
    
    return;
}

void idt_set_descriptor(u8 vector, void* isr, u8 flags) {
    idt_entry_t* descriptor = &idt[vector];

    descriptor->isr_low = (u64)isr & 0xFFFF;
    descriptor->kernel_cs = 0x08;
    descriptor->ist = 0;
    descriptor->attributes = flags;
    descriptor->isr_mid = ((u64)isr >> 16) & 0xFFFF;
    descriptor->isr_high = ((u64)isr >> 32) & 0xFFFFFFFF;
    descriptor->reserved = 0;
}

#define IDT_MAX_DESCRIPTORS 255
static bool vectors[IDT_MAX_DESCRIPTORS];

extern void* isr_stub_table[];
extern void* irq_stub_table[];

void (*irq_routines[16])(void) = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
void irq_handler(u64 idx) {
    void (*routine)(void) = irq_routines[idx];
    if(routine) {
        routine();
    }
    
    if(idx >= 8) {
        out_port(0xA0, 0x20);
    }

    out_port(0x20, 0x20);
}

void irq_install(u64 irq, void(*handler)(void)) {
    irq_routines[irq] = handler; 
}

void irq_remove(u64 irq) {
    irq_routines[irq] = 0; 
}

void idt_init() {
    idtr.base = (ptr_64)&idt[0];
    idtr.limit = (u16)sizeof(idt_entry_t) * IDT_MAX_DESCRIPTORS - 1;

    for (u8 vector = 0; vector < 32; vector++) {
        idt_set_descriptor(vector, isr_stub_table[vector], 0x8E);
        vectors[vector] = 1;
    }

    for (u8 vector = 32; vector < 48; vector++) {
        idt_set_descriptor(vector, irq_stub_table[vector-32], 0x8E);
        vectors[vector] = 1;
    }

    out_port(0x20, 0x11);
    out_port(0xA0, 0x11);

    out_port(0x21, 0x20);
    out_port(0xA1, 0x28);

    out_port(0x21,0x04);
    out_port(0xA1,0x02);

    out_port(0x21, 0x01);
    out_port(0xA1, 0x01);

    out_port(0x21, 0x0);
    out_port(0xA1, 0x0);


    // out_port(0x21, 0xff);
    // out_port(0xa1, 0xff);

    __asm__ volatile("lidt %0" : : "m"(idtr));  // load the new IDT
    __asm__ volatile("sti");                    // set the interrupt flag
    set_color(White, Green);
    printf("IDT Initialized\n");
    set_color(LightGray, Black);
}

