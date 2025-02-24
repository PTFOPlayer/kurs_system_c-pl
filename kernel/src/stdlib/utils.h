#pragma once
#include "stdint.h"

#define swap(x, y)        \
    {                     \
        typeof(x) _x = x; \
        typeof(y) _y = y; \
        x = _y;           \
        y = _x;           \
    }

void itoa(i64 ivalue, char *dest, i64 base) {
    u32 idx = 0;

    u8 neg = 0;
    if (base == 10 && ivalue < 0) {
        neg = 1;
        ivalue = -ivalue;
    }
    u64 value = *((u64 *)&ivalue);

    char *chars = "abcdefghijklmnoprstuvwxyz";
    while (value != 0) {
        i64 temp = value % base;

        if (temp < 10) {
            *(dest + idx) = temp + '0';
        } else {
            *(dest + idx) = chars[temp - 10];
        }

        value /= base;
        idx++;
    }

    if (neg) {
        *(dest + idx) = '-';
        idx++;
    }

    for (u32 i = 0; i < idx / 2; i++) swap(dest[i], dest[idx - i - 1]);
}