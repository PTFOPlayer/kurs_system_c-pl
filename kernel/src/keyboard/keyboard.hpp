#pragma once
#include "../interrupts/interrupts.hpp"
#include "./scancodes.hpp"
#include "../utils/utils.hpp"
#include <stdint.h>

uint8_t caps = 0;
uint8_t capslock = 0;

void keyboard(IRQFrame*) {
    char scan = in_b(0x60) & 0x7f;
    char press = in_b(0x60) & 0x80;
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
                    printf("%c", uppercase[(int)scan]);
                } else {
                    printf("%c", lowercase[(int)scan]);
                }
            }
    }
}

void keyboard_init() {
    caps = 0;
    capslock = 0;
    irq_install(1, keyboard);
    info("PS/2 Keyboard Initialized\n");
}