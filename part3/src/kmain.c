#include "interrupts/interrupts.h"
#include "keyboard/keyboard.h"
#include "vga/colors.h"
#include "vga/vga.h"
void kmain() {
    init_writer(LightGray, Black);
    idt_init();
    init_keyboard();
    puts("hello from c");
    putnl();
    puts("its basic os");
    for (int i = 0; i < 23; i++) {
        putnl();
    }
    puts("buffer should shift");
    putnl();
    puts("if you see this buffer shifted");

    // test przerwań

    // dzielenie przez zero

    // i64 b = 0;
    // i64 a = 1;
    // a = a / b;
    // if (a);

    while (1) {}
}
