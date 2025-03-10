#pragma once
#include <stdint.h>

struct LinkedListNode {
    LinkedListNode* next;
};

class LinkedListAllocator {
   private:
    uint32_t block_size;
    uint32_t memory_capacity;
    uint32_t memory_used = 0;
    void* base;
    LinkedListNode* tail;

   public:
    LinkedListAllocator(void* base, uint32_t capacity, uint32_t block_size);
    ~LinkedListAllocator();

    void* ll_malloc(uint32_t);
    static void ll_free(void*);
};

LinkedListAllocator::LinkedListAllocator(void* base, uint32_t capacity,
                                         uint32_t block_size) {
    this->block_size = block_size;
    this->memory_capacity = capacity;
    this->base = base;
    this->tail = (LinkedListNode*)base;
    tail->next = nullptr;
}

LinkedListAllocator::~LinkedListAllocator() {}

void* LinkedListAllocator::ll_malloc(uint32_t size) {
    LinkedListNode* alloc = nullptr;

    void* addr = (char*)this->tail + sizeof(LinkedListNode);

    if (size + memory_used > memory_capacity) {
        return nullptr;
    }

    if (tail->next == nullptr) {
        alloc = (LinkedListNode*)((char*)this->tail + sizeof(LinkedListNode) +
                                  this->block_size);
        alloc->next = nullptr;
        tail->next = alloc;
    } else {
        alloc = tail->next;
    }
    tail = alloc;
    return addr;
}

void LinkedListAllocator::ll_free(void* addr) {
    // placeholder
}