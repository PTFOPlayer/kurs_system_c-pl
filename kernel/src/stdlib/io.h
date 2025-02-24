#pragma once
#include "stdint.h"

void out_port(word port, byte b) {
    asm volatile("out %1, %0" : : "dN"(port), "a"(b));
}

byte in_port(word port) {
    byte b;
    asm volatile("in %1, %0" : "=a"(b) : "dN"(port));
    return b;
}