#pragma once
#include <stdint.h>

#define swap(x, y)        \
    {                     \
        typeof(x) _x = x; \
        typeof(y) _y = y; \
        x = _y;           \
        y = _x;           \
    }

void itoa(int64_t ivalue, char *dest, int64_t base) {
    if (ivalue == 0) {
        *dest = '0';
        return;
    }

    int32_t idx = 0;

    uint8_t neg = 0;
    if (base == 10 && ivalue < 0) {
        neg = 1;
        ivalue = -ivalue;
    }
    uint64_t value = *((uint64_t *)&ivalue);

    char *chars = "abcdefghijklmnoprstuvwxyz";
    while (value != 0) {
        int64_t temp = value % base;

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

    for (int32_t i = 0; i < idx / 2; i++) swap(dest[i], dest[idx - i - 1]);
}

void *memset(void *s, uint8_t c, size_t n) {
    uint8_t *p = (uint8_t *)s;

    for (size_t i = 0; i < n; i++) {
        p[i] = c;
    }

    return s;
}

void out_b(uint16_t port, char b) {
    asm volatile("outb %1, %0" : : "d"(port), "a"(b));
}

char in_b(uint16_t port) {
    char b;
    asm volatile("inb %1, %0" : "=a"(b) : "d"(port));
    return b;
}

void out_w(uint16_t port, uint16_t b) {
    asm volatile("outw %1, %0" : : "d"(port), "a"(b));
}

uint16_t in_w(uint16_t port) {
    uint16_t b;
    asm volatile("inw %1, %0" : "=a"(b) : "d"(port));
    return b;
}

void out_dw(uint16_t port, uint32_t b) {
    asm volatile("outl %1, %0" : : "d"(port), "a"(b));
}

uint32_t in_dw(uint16_t port) {
    uint32_t b;
    asm volatile("inl %1, %0" : "=a"(b) : "d"(port));
    return b;
}