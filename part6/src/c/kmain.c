
#include "cpuid/cpuid.h"
#include "int.h"
#include "keyboard.h"
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

    char num_buff[10] = {0};

    for (u8 i = 0; i < 10; i++) {
        for (u8 i = 0; i < 10; i++) num_buff[i] = 0;

        itoa((i64)(prand(i) % 10000000), num_buff, 10);
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