#include "int.h"
// function-like macro
#define swap(x, y)        \
    {                     \
        typeof(x) _x = x; \
        typeof(y) _y = y; \
        x = _y;           \
        y = _x;           \
    }

void itoa(i64 value, char *dest, i64 base)
{
    u32 idx = 0;
    while (value > 0)
    {
        *(dest + idx) = (value % base) + '0';
        value /= base;
        idx++;
    }

    for (u32 i = 0; i < idx / 2; i++)
        swap(dest[i], dest[idx - i - 1]);
}

// prosta funkcja hashująca
u32 prand(u32 seed) {
    seed = (seed ^ 61) ^ (seed >> 16);
    seed = seed + (seed << 3);
    seed = seed ^ (seed >> 4);
    seed = seed * 0x27d4eb2d;
    seed = seed ^ (seed >> 15);
    return seed;
}

void memcpy(u8 *src, u8 *dest, u32 len) {
    for (u32 i = 0; i < len; i++)
        dest[i] = src[i];
}

void meminit(u8 byte , u8 *dest, u32 len) {
    for (u32 i = 0; i < len; i++)
        dest[i] = byte;
}

void outPort(u16 port, u8 byte) {
    asm volatile("out %1, %0" : : "dN" (port), "a" (byte) );
}

char inPort(u16 port) {
    char byte; 
    asm volatile("in %1, %0" : "=a" (byte) : "dN" (port) );
    return byte;
}