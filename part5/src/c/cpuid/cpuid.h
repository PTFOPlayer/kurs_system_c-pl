#pragma once

#include "../int.h"

static inline u32 cpuid(u32 code, u32 regs[4])
{
    asm volatile("cpuid" : "=a"(*regs), "=b"(*(regs + 1)),
                           "=c"(*(regs + 2)), "=d"(*(regs + 3)) : "a"(code));
    return (int)regs[0];
}

// funkcja zwraca nazwe procesora
void cpu_name(char buff[48])
{
    u32 regs[12];
    cpuid(0x80000002, regs);
    cpuid(0x80000003, regs + 4);
    cpuid(0x80000004, regs + 8);

    char *str = (char *)regs;
    for (int i = 0; i < 48; i++)
        buff[i] = str[i];
}