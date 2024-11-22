#pragma once
#include "int.h"
// function-like macro
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

// prosta funkcja hashująca
u64 prand(u64 seed) {
    seed = (seed ^ 61) ^ (seed >> 16);
    seed = seed + (seed << 3);
    seed = seed ^ (seed >> 4);
    seed = seed * 0x27d4eb2d;
    seed = seed ^ (seed >> 15);
    return seed;
}

void memcpy(u8 *src, u8 *dest, u32 len) {
    for (u32 i = 0; i < len; i++) dest[i] = src[i];
}

void meminit(u8 byte, u8 *dest, u32 len) {
    for (u32 i = 0; i < len; i++) dest[i] = byte;
}

void outPort(u16 port, u8 byte) {
    asm volatile("out %1, %0" : : "dN"(port), "a"(byte));
}

char inPort(u16 port) {
    char byte;
    asm volatile("in %1, %0" : "=a"(byte) : "dN"(port));
    return byte;
}