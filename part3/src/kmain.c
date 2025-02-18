#include "vga/colors.h"
#include "vga/vga.h"
#include "interrupts/interrupts.h"
void kmain() {
    init_writer(LightGray, Black);
    idt_init();
    puts("hello from c");
    putnl();
    puts("its basic os");
    for (int i = 0; i < 23; i++)
    {
        putnl();
    }
    puts("buffer should shift");
    putnl();
    puts("if you see this buffer shifted");
    
    while (1) {}


    // test przerwań
    int a = 1 / 0;
    if (a)
        ;
    asm volatile("cli");
    while (1) {
    }

    asm("hlt");
}