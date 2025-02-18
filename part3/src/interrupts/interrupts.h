#include "../stdlib/stdint.h"
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

void exception_handler(void);
void exception_handler() {
    char* buff = (char*)0xb8000 + 160;
    set_color(Red, Yellow);
    char* hello = "interrupt occured";
    puts(hello);
    asm volatile("cli");
    return;
}

void idt_set_descriptor(u8 vector, void* isr, u8 flags);
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
static u8 vectors[IDT_MAX_DESCRIPTORS];  // bool

extern void* isr_stub_table[];

void idt_init(void);
void idt_init() {
    idtr.base = (ptr_64)&idt[0];
    idtr.limit = (u16)sizeof(idt_entry_t) * IDT_MAX_DESCRIPTORS - 1;

    for (u8 vector = 0; vector < 32; vector++) {
        idt_set_descriptor(vector, isr_stub_table[vector], 0x8E);
        vectors[vector] = 1;
    }

    __asm__ volatile("lidt %0" : : "m"(idtr));  // load the new IDT
    __asm__ volatile("sti");                    // set the interrupt flag
}