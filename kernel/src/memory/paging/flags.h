#pragma once
#include "../../stdlib/stdint.h"
enum EntryFlags {
    PAGE_PRESENT = 1 << 0,
    PAGE_WRITABLE = 1 << 1,
    PAGE_USER_ACCESSIBLE = 1 << 2,
    PAGE_WRITE_THROUGH = 1 << 3,
    PAGE_NO_CACHE = 1 << 4,
    PAGE_ACCESSED = 1 << 5,
    PAGE_DIRTY = 1 << 6,
    PAGE_HUGE_PAGE = 1 << 7,
    PAGE_GLOBAL = 1 << 8,
    // unsigned long long
    PAGE_NO_EXECUTE = 1ull << 63,
};

typedef struct EntryFlagsStruct {
    bool PAGE_PRESENT;
    bool PAGE_WRITABLE;
    bool PAGE_USER_ACCESSIBLE;
    bool PAGE_WRITE_THROUGH;
    bool PAGE_NO_CACHE;
    bool PAGE_ACCESSED;
    bool PAGE_DIRTY;
    bool PAGE_HUGE_PAGE;
    bool PAGE_GLOBAL;
    bool PAGE_NO_EXECUTE;
} EntryFlagsStruct;
