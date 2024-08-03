
#include "int.h"
#include "vga/vga.h"
#include "cpuid/cpuid.h"

void kmain()
{
    // testowanie wypisywania na VGA
    kclear();
    kputs("hello from c");
    kputc('!');
    kputnl();
    kputnl();
    kputs("lorem ipsum dolor");
    kputnl();
    kputnl();

    char buff[48] = {0};
    cpu_name(buff);
    kputs(buff);

    asm("hlt");
}