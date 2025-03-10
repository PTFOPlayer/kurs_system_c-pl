#pragma once
#include <stdint.h>

#include "ll_allocator.hpp"

struct LLHeapRequest {
    uint32_t block_size;
    uint32_t block_num;
};

static LinkedListAllocator* allocators[13];

void ll_heap_init(LLHeapRequest* list, uint32_t length, void* base) {
    char* ptr = (char*)base;
    for (uint32_t i = 0; i < length; i++) {
        *(LinkedListAllocator*)ptr =
            LinkedListAllocator((char*)base + sizeof(LinkedListAllocator),
                                list[i].block_size, list[i].block_num);

        allocators[i] = (LinkedListAllocator*)ptr;
        ptr +=
            sizeof(LinkedListAllocator) +
            list[i].block_num * (list[i].block_size + sizeof(LinkedListNode));
    }
}

uint32_t to_ll_indx(uint32_t length) {
    uint32_t pow = 1;
    uint32_t idx = 0;
    while (pow <= length) {
        idx++;
        pow *= 2;
    }
    return idx;
}

void init_heap(void* base) {
    LLHeapRequest requests[] = {{.block_size = 1, .block_num = 1000},
                                {.block_size = 2, .block_num = 1000},
                                {.block_size = 4, .block_num = 1000},
                                {.block_size = 8, .block_num = 1000},
                                {.block_size = 16, .block_num = 1000},
                                {.block_size = 32, .block_num = 1000},
                                {.block_size = 64, .block_num = 1000},
                                {.block_size = 128, .block_num = 1000},
                                {.block_size = 256, .block_num = 1000},
                                {.block_size = 512, .block_num = 1000},
                                {.block_size = 1024, .block_num = 1000},
                                {.block_size = 2048, .block_num = 1000},
                                {.block_size = 4096, .block_num = 1000}};
    ll_heap_init(requests, sizeof(requests) / sizeof(LLHeapRequest), base);
}

void* malloc(uint32_t length) {
    if (length == 0) {
        return nullptr;
    }
    uint32_t pos = to_ll_indx(length);
    return allocators[pos]->ll_malloc(length);
}

void free(void* addr) { LinkedListAllocator::ll_free(addr); }