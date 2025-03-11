#pragma once
#include <stdint.h>

#include "../graphics/graphics.hpp"

struct LinkedListNode {
    LinkedListNode* next;
    uint32_t size;
};

class LinkedListAllocator {
   private:
    LinkedListNode* head;
    LinkedListNode* tail;
    LinkedListNode* free_list_head;
    LinkedListNode* free_list_tail;

   public:
    LinkedListAllocator(void* base);
    ~LinkedListAllocator() {};

    void dbg(TextMode& text_mode);
    void* malloc(uint32_t);
    void free(void*);
};

LinkedListAllocator::LinkedListAllocator(void* base) {
    this->head = (LinkedListNode*)base;
    this->tail = this->head;
    this->tail->next = nullptr;
    this->tail->size = 0;
    this->free_list_head = nullptr;
    this->free_list_tail = nullptr;
}

void* LinkedListAllocator::malloc(uint32_t size) {
    LinkedListNode* new_node =
        (LinkedListNode*)((uint64_t)this->tail + sizeof(LinkedListNode) +
                          this->tail->size);

    LinkedListNode* prev = nullptr;
    LinkedListNode* current = free_list_head;

    while (current) {
        if (current->size >= size) {
            if (prev) {
                prev->next = current->next;
            } else {
                free_list_head = current->next;
            }

            if (current == free_list_tail) {
                free_list_tail = prev;
            }

            return (void*)((uint64_t)current + sizeof(LinkedListNode));
        }
        prev = current;
        current = current->next;
    }

    if ((uintptr_t)new_node < (uintptr_t)this->tail) {
        return nullptr;
    }

    this->tail->next = new_node;
    this->tail = new_node;
    this->tail->next = nullptr;
    this->tail->size = size;

    return (void*)((uint64_t)this->tail + sizeof(LinkedListNode));
}

void LinkedListAllocator::free(void* ptr) {
    if (!ptr) return;

    LinkedListNode* node =
        (LinkedListNode*)((uint64_t)ptr - sizeof(LinkedListNode));

    if (free_list_head == nullptr) {
        free_list_head = node;
        free_list_tail = node;
        node->next = nullptr;
    } else {
        node->next = free_list_head;
        free_list_head = node;
    }
}

void LinkedListAllocator::dbg(TextMode& text_mode) {
    text_mode.info("\n\nAllocated nodes:\n");
    LinkedListNode* head = this->head;
    while (head != nullptr) {
        text_mode.printf("\t Base: %d, Size: %d\n", head, head->size);
        head = head->next;
    }

    text_mode.info("Freed nodes:\n");
    LinkedListNode* freed = this->free_list_head;
    while (freed != nullptr) {
        text_mode.printf("\t Base: %d, Size: %d\n", freed, freed->size);
        freed = freed->next;
    }
}