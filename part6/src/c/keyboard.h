#include "int.h"

const char OTHER = 0;

const char keys[128] = {
    OTHER, OTHER, '1',   '2',   '3',   '4',   '5',   '6',   '7',   '8',   '9',
    '0',   '-',   '=',   '\b',  '\t',  'q',   'w',   'e',   'r',   't',   'y',
    'u',   'i',   'o',   'p',   '[',   ']',   OTHER, OTHER, 'a',   's',   'd',
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