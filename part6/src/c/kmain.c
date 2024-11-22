
#include "cpuid/cpuid.h"
#include "int.h"
#include "keyboard/keyboard.h"
#include "stdlib.h"
#include "vga/vga.h"

void kmain() {
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

    kputnl();
    
    char num_buff[32] = {0};
    meminit(0, (u8*)num_buff, 32);

    for (u8 i = 0; i < 16; i++) {
        meminit(0, (u8*)num_buff, 32);
        itoa(i, num_buff, 16);
        kputs(num_buff);
        kputc(' ');
    }
    kputnl();
    for (u8 i = 0; i < 10; i++) {
        meminit(0, (u8*)num_buff, 32);
        itoa(((i64)prand(i) % 10000000000) - 5000000000, num_buff, 16);
        kputs(num_buff);
        kputnl();
    }

    init_keyboard();

    while (1) {
        char key = scan_keyboard();
        if (key != OTHER) {
            kputc(key);
        }
    }

    asm("hlt");
}