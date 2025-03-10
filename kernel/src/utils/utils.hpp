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
