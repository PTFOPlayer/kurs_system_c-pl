#pragma once
#include "../../stdlib/assert.h"
#include "../../stdlib/stdint.h"
#include "../frame_allocator/frame.h"
#include "../frame_allocator/frame_allocator.h"
#include "flags.h"
#include "page_table.h"

const PageTable *p4 = (PageTable *)0xfffffffffffff000;


bool next_table_address(PageTable *page_table, u64 idx, u64 *page_table_addr) {
    EntryFlagsStruct flags = entry_flags(&page_table->entries[idx]);
    if (flags.PAGE_PRESENT && !flags.PAGE_HUGE_PAGE)
    {
        u64 table_addr = (u64)page_table;
        *page_table_addr = (table_addr << 9) | (idx << 12);
        return true;
    }
    return false;   
}

bool next_table(PageTable *page_table, u64 idx, PageTable * next_page_table) {
    return next_table_address(page_table, idx, (u64*)next_page_table);
}