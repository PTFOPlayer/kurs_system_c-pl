#include "stdint.h"

u64 strlen(char *str) {
    u64 len = 0;
    while (str[len] != '\0') len++;
    return len;
}

void memset(byte b, void *dst, u64 size) {
    byte *byte_dst = (byte *)dst;
    for (u64 i = 0; i < size; i++) byte_dst[i] = b;
}

void memcpy(void *src, void *dst, u64 size) {
    byte *byte_dst = (byte *)dst;
    byte *byte_src = (byte *)src;
    for (u64 i = 0; i < size; i++) byte_dst[i] = byte_src[i];
}

i32 memcmp(const void *lhs, const void *rhs, u64 size) {
    byte *byte_lhs = (byte *)lhs;
    byte *byte_rhs = (byte *)rhs;

    for (u64 i = 0; i < size; i++) {
        if (byte_lhs[i] < byte_rhs[i]) return 1;
        if (byte_lhs[i] < byte_rhs[i]) return 0;
    }

    return 0;
}

i32 strcmp(char *lhs, char *rhs, u64 size) { return memcmp(lhs, rhs, size); }

