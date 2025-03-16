#pragma once
#include <stddef.h>
#include "ll_allocator.hpp"

void *operator new(size_t size) {
    return malloc(size);
}

void *operator new[](size_t size) {
    return malloc(size);
}

void operator delete(void *p) {
    free(p);
}

void operator delete[](void *p) {
    free(p);
}

void operator delete(void *p, long unsigned int) {
    free(p);
}

void operator delete[](void *p, long unsigned int) {
    free(p);
}