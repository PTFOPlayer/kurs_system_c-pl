
#include "int.h"
#include "vga/vga.h"
void kmain()
{
    // testowanie wypisywania na VGA
    kclear();
    kputs("hello from c");
    kputc('!');
    kputnl();
    kputnl();
    kputs("lorem ipsum dolor");

    asm("hlt");
}