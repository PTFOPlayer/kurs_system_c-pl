#pragma once
#include "../../stdlib/assert.h"
#include "../../stdlib/stdint.h"
#include "../frame_allocator/frame.h"
#include "../frame_allocator/frame_allocator.h"
#include "flags.h"

#define ENTRY 512

typedef struct Entry {
    u64 number;
} Entry;

bool is_unused(Entry *e) { return e->number == 0; }

void set_unused(Entry *e) { e->number = 0; }

EntryFlagsStruct entry_flags(Entry *e) {
    EntryFlagsStruct flags;
    flags.PAGE_PRESENT = (e->number & PAGE_PRESENT) != 0;
    flags.PAGE_WRITABLE = (e->number & PAGE_WRITABLE) != 0;
    flags.PAGE_USER_ACCESSIBLE = (e->number & PAGE_USER_ACCESSIBLE) != 0;
    flags.PAGE_WRITE_THROUGH = (e->number & PAGE_WRITE_THROUGH) != 0;
    flags.PAGE_NO_CACHE = (e->number & PAGE_NO_CACHE) != 0;
    flags.PAGE_ACCESSED = (e->number & PAGE_ACCESSED) != 0;
    flags.PAGE_DIRTY = (e->number & PAGE_DIRTY) != 0;
    flags.PAGE_HUGE_PAGE = (e->number & PAGE_HUGE_PAGE) != 0;
    flags.PAGE_GLOBAL = (e->number & PAGE_GLOBAL) != 0;
    flags.PAGE_NO_EXECUTE = (e->number & PAGE_NO_EXECUTE) != 0;
    return flags;
}

bool pointed_frame(Entry *e, Frame *frame) {
    if (e->number & PAGE_PRESENT) {
        *frame = containing_address(e->number & 0x000ffffffffff000);
        return true;
    }
    return false;
}

void set_entry(Entry *e, Frame frame, u64 flags) {
    u64 frame_start = frame_start_address(frame);
    assert_eq((frame_start & ~0x000ffffffffff000), 0);
    e->number = frame_start | flags;
}

typedef struct PageTable {
    Entry entries[ENTRY];
} PageTable;

void zero_page(PageTable *page_table) {
    for (i32 i = 0; i < ENTRY; i++) {
        set_unused(&page_table->entries[i]);
    }
}


