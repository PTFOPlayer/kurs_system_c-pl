
#include "cpuid/cpuid.h"
#include "int.h"
#include "stdlib.h"
#include "vga/vga.h"
#include "keyboard.h"

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

    u16 ps2_io_port = 0x60;
    u16 ps2_control_port = 0x64;
    outPort(ps2_control_port, 0xFF);


    char last = 0;
    u16 cnt = 0;
    while (1) {
        if (!cnt) {
            unsigned char input = inPort(ps2_io_port);
            unsigned char scan = input & 0x7F;
            if (scan != last)
            {   
                last = scan;
                kputc(keys[scan]);
            }
        }
        cnt ++;
    }

    asm("hlt");
}