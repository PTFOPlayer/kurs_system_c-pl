#include "int.h"
#include "stdlib.h"
const char OTHER = 255;

const char keys[128] = {
    OTHER, OTHER, '1',   '2',   '3',   '4',   '5',   '6',   '7',   '8',   '9',
    '0',   '-',   '=',   '\b',  '\t',  'q',   'w',   'e',   'r',   't',   'y',
    'u',   'i',   'o',   'p',   '[',   ']',   '\n',  OTHER, 'a',   's',   'd',
    'f',   'g',   'h',   'j',   'k',   'l',   ';',   '\'',  '`',   OTHER, '\\',
    'z',   'x',   'c',   'v',   'b',   'n',   'm',   ',',   '.',   '/',   OTHER,
    '*',   OTHER, ' ',   OTHER, OTHER, OTHER, OTHER, OTHER, OTHER, OTHER, OTHER,
    OTHER, OTHER, OTHER, OTHER, OTHER, OTHER, OTHER, OTHER, OTHER, OTHER, OTHER,
    OTHER, OTHER, OTHER, OTHER, OTHER, OTHER, OTHER, OTHER, OTHER, OTHER, OTHER,
    OTHER, OTHER, OTHER, OTHER, OTHER, OTHER, OTHER, OTHER, OTHER, OTHER, OTHER,
    OTHER, OTHER, OTHER, OTHER, OTHER, OTHER, OTHER, OTHER, OTHER, OTHER, OTHER,
    OTHER, OTHER, OTHER, OTHER, OTHER, OTHER, OTHER, OTHER, OTHER, OTHER, OTHER,
    OTHER, OTHER, OTHER, OTHER, OTHER, OTHER, OTHER};

const char upper[128] = {
    OTHER, OTHER, '!',   '@',   '#',   '$',   '%',   '^',   '&',   '*',   '(',
    ')',   '_',   '+',   '\b',  '\t',  'Q',   'W',   'E',   'R',   'T',   'Y',
    'U',   'I',   'O',   'P',   '{',   '}',   '\n',  OTHER, 'A',   'S',   'D',
    'F',   'G',   'H',   'J',   'K',   'L',   ':',   '"',   '~',   OTHER, '|',
    'Z',   'X',   'C',   'V',   'B',   'N',   'M',   '<',   '>',   '?',   OTHER,
    '*',   OTHER, ' ',   OTHER, OTHER, OTHER, OTHER, OTHER, OTHER, OTHER, OTHER,
    OTHER, OTHER, OTHER, OTHER, OTHER, OTHER, OTHER, OTHER, '-',   OTHER, OTHER,
    OTHER, '+',   OTHER, OTHER, OTHER, OTHER, OTHER, OTHER, OTHER, OTHER, OTHER,
    OTHER, OTHER, OTHER, OTHER, OTHER, OTHER, OTHER, OTHER, OTHER, OTHER, OTHER,
    OTHER, OTHER, OTHER, OTHER, OTHER, OTHER, OTHER, OTHER, OTHER, OTHER, OTHER,
    OTHER, OTHER, OTHER, OTHER, OTHER, OTHER, OTHER, OTHER, OTHER, OTHER, OTHER,
    OTHER, OTHER, OTHER, OTHER, OTHER, OTHER, OTHER};

const u8 scan_mask = 0b01111111;
const u16 ps2_io_port = 0x60;
const u16 ps2_control_port = 0x64;

void init_keyboard() { outPort(ps2_control_port, 0xFF); }

char scan_keyboard() {
    u8 status = inPort(ps2_control_port);
    if (status & 1) {
        u8 input = inPort(ps2_io_port);
        u8 scan = input & scan_mask;
        u8 pressed = input & ~scan_mask;
        if (pressed) {
            return keys[scan];
        }
    }

    return OTHER;
}