#include "vga/colors.h"
#include "vga/vga.h"
void kmain() {
    init_writer(LightGray, Black);
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

    asm("hlt");
}