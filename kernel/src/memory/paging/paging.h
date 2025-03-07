#pragma once
#include "../../stdlib/assert.h"
#include "../../stdlib/stdint.h"
#include "../frame_allocator/frame.h"
#include "../frame_allocator/frame_allocator.h"
#include "flags.h"
#include "page_table.h"

const PageTable *P4_GLOBAL = (PageTable *)0xfffffffffffff000;

bool next_table_address(PageTable *page_table, u64 idx, u64 *page_table_addr) {
    EntryFlagsStruct flags = entry_flags(&page_table->entries[idx]);
    if (flags.PAGE_PRESENT && !flags.PAGE_HUGE_PAGE) {
        u64 table_addr = (u64)page_table;
        *page_table_addr = ((table_addr << 9) | (idx << 12));
        return true;
    }
    return false;
}

bool next_table(PageTable *page_table, u64 idx, PageTable **next_page_table) {
    u64 addr = 0;
    if (next_table_address(page_table, idx, &addr)) {
        *next_page_table = (PageTable *)addr;
        return true;
    }
    return false;
}

typedef struct Page {
    u64 number;
} Page;

u64 p4_index(Page page) { return (page.number >> 27) & 0x1ff; }
u64 p3_index(Page page) { return (page.number >> 18) & 0x1ff; }
u64 p2_index(Page page) { return (page.number >> 9) & 0x1ff; }
u64 p1_index(Page page) { return (page.number >> 0) & 0x1ff; }

Page page_containing_address(u64 virtual) {
    Page p = {virtual / PAGE_SIZE};
    return p;
}

bool translate_page(Page page, Frame *frame) {
    PageTable *p4 = P4_GLOBAL;
    PageTable *p3, *p2, *p1;

    if (!next_table(p4, p4_index(page), &p3)) {
        Entry *p3_entry = &p3->entries[p3_index(page)];
        Frame start_frame;
        if (pointed_frame(p3_entry, &start_frame) &&
            entry_flags(p3_entry).PAGE_HUGE_PAGE) {
            // align to 1GiB
            assert_eq(start_frame.number % (ENTRY * ENTRY), 0);
            Frame temp = {.number = start_frame.number +
                                    p2_index(page) * ENTRY  +p1_index(page)};
            *frame = temp;
            return true;
        }
        return false;
    };

    if (!next_table(p3, p3_index(page), &p2)) {
        Entry *p2_entry = &p2->entries[p2_index(page)];
        Frame start_frame;
        if (pointed_frame(p2_entry, &start_frame) &&
            entry_flags(p2_entry).PAGE_HUGE_PAGE) {
            // align to 2MiB
            assert_eq(start_frame.number % ENTRY, 0);
            Frame temp = {.number = start_frame.number + p1_index(page)};
            *frame = temp;
            return true;
        } else {
            return false;
        }
    };
    if (!next_table(p2, p2_index(page), &p1)) {
        return false;
    };

    return pointed_frame(&p1->entries[p1_index(page)], frame);
}

bool translate_address(u64 virtual, u64 *physical) {
    u64 offset = virtual % PAGE_SIZE;
    Frame f;
    if (translate_page(page_containing_address(virtual), &f)) {
        *physical = f.number * PAGE_SIZE + offset;
        return true;
    }
    return false;
}

void create_next_table(PageTable *page_table, u64 idx,
                       AreaFrameAllocator *allocator, PageTable **next_page) {
    if (!next_table(page_table, idx, next_page)) {
        Frame f;
        assert(allocate_frame(allocator, &f));
        set_entry(&page_table->entries[idx], f, PAGE_PRESENT | PAGE_WRITABLE);
        assert(next_table(page_table, idx, next_page));
        zero_page(*next_page);
    }
}

void map_to(Page page, Frame frame, u64 flags, AreaFrameAllocator *allocator) {
    PageTable *p4 = P4_GLOBAL;
    PageTable *p3;
    PageTable *p2;
    PageTable *p1;

    create_next_table(p4, p4_index(page), allocator, &p3);
    create_next_table(p3, p3_index(page), allocator, &p2);
    create_next_table(p2, p2_index(page), allocator, &p1);
    set_entry(&p1->entries[p1_index(page)], frame, flags | PAGE_PRESENT);
}