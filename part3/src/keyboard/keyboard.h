#pragma once
#include "../interrupts/interrupts.h"
#include "./scancodes.h"

u8 caps = 0;
u8 capslock = 0;

void keyboard() {
    char scan = in_port(0x60) & 0x7f;
    char press = in_port(0x60) & 0x80;
    switch (scan) {
        case 1:
        case 29:
        case 56:
        case 59:
        case 60:
        case 61:
        case 62:
        case 63:
        case 64:
        case 65:
        case 66:
        case 67:
        case 68:
        case 87:
        case 88:
            break;
        case 42:
            if (press == 0) {
                caps = 1;
            } else {
                caps = 0;
            }
            break;
        case 58:
            if (!caps && press == 0) {
                capslock = 1;
            } else if (capslock && press == 0) {
                capslock = 0;
            }
            break;
        default:
            if (!press) {
                if (caps || capslock) {
                    putc(uppercase[scan]);
                } else {
                    putc(lowercase[scan]);
                }
            }
    }
}

void init_keyboard() {
    caps = 0;
    capslock = 0;
    irq_install(1, keyboard);
    set_color(White, Green);
    puts("PS/2 Keyboard Initialized\n");
    set_color(LightGray, Black);
}
