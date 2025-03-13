#pragma once
#include <stdint.h>

#include "../graphics/graphics.hpp"
#include "../limine.h"
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

    void dbg();
    friend void* malloc(uint32_t);
    friend void free(void*);
};

static LinkedListAllocator* ll_allocator = nullptr;
LinkedListAllocator::LinkedListAllocator(void* base) {
    ll_allocator = this;
    this->head = (LinkedListNode*)base;
    this->tail = this->head;
    this->tail->next = nullptr;
    this->tail->size = 0;
    this->free_list_head = nullptr;
    this->free_list_tail = nullptr;
}

void* malloc(uint32_t size) {
    LinkedListNode* new_node =
        (LinkedListNode*)((uint64_t)ll_allocator->tail +
                          sizeof(LinkedListNode) + ll_allocator->tail->size);

    LinkedListNode* prev = nullptr;
    LinkedListNode* current = ll_allocator->free_list_head;

    while (current) {
        if (current->size >= size) {
            if (prev) {
                prev->next = current->next;
            } else {
                ll_allocator->free_list_head = current->next;
            }

            if (current == ll_allocator->free_list_tail) {
                ll_allocator->free_list_tail = prev;
            }

            return (void*)((uint64_t)current + sizeof(LinkedListNode));
        }
        prev = current;
        current = current->next;
    }

    if ((uintptr_t)new_node < (uintptr_t)ll_allocator->tail) {
        return nullptr;
    }

    ll_allocator->tail->next = new_node;
    ll_allocator->tail = new_node;
    ll_allocator->tail->next = nullptr;
    ll_allocator->tail->size = size;

    return (void*)((uint64_t)ll_allocator->tail + sizeof(LinkedListNode));
}

void free(void* ptr) {
    if (!ptr) return;

    LinkedListNode* node =
        (LinkedListNode*)((uint64_t)ptr - sizeof(LinkedListNode));

    if (ll_allocator->free_list_head == nullptr) {
        ll_allocator->free_list_head = node;
        ll_allocator->free_list_tail = node;
        node->next = nullptr;
    } else {
        node->next = ll_allocator->free_list_head;
        ll_allocator->free_list_head = node;
    }
}

void LinkedListAllocator::dbg() {
    info("\n\nAllocated nodes:\n");
    LinkedListNode* head = this->head;
    while (head != nullptr) {
        printf("\t Base: %d, Size: %d\n", head, head->size);
        head = head->next;
    }

    info("Freed nodes:\n");
    LinkedListNode* freed = this->free_list_head;
    while (freed != nullptr) {
        printf("\t Base: %d, Size: %d\n", freed, freed->size);
        freed = freed->next;
    }
}

limine_memmap_entry* find_first_valid_mmap(limine_memmap_response* response) {
    limine_memmap_entry* current_entry = nullptr;
    for (size_t i = 0; i < response->entry_count; i++) {
        limine_memmap_entry* entry = response->entries[i];
        if (entry->type == LIMINE_MEMMAP_USABLE && current_entry == nullptr) {
            current_entry = entry;
        }
    }
    return current_entry;
}

void* get_base(limine_memmap_entry* entry, limine_hhdm_response* hhdm) {
    return (void*)(entry->base + hhdm->offset);
}

void print_mmap(limine_memmap_response* response) {
    for (size_t i = 0; i < response->entry_count; i++) {
        limine_memmap_entry* entry = response->entries[i];
        printf("\tBase: 0x%x\t|\tLen: %x", entry->base, entry->length);
        printf("\n");
    }
}