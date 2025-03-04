#pragma once
#include "../multiboot/multiboot.h"
#include "../multiboot/multiboot_parse.h"
#include "../stdlib/stdint.h"
#define PAGE_SIZE 0x1000  // 4096

typedef struct Frame {
    u64 number;
} Frame;

Frame containing_address(u64 address) {
    Frame frame = {address / PAGE_SIZE};
    return frame;
}


typedef struct AreaFrameAllocator {
    Frame next_free_frame;
    multiboot_memory_map_t *current_area;
    // lifetime tied to MultibootData
    multiboot_memory_map_t *mmap;
    u64 mmap_size;
    struct boundries {
        u64 multiboot_start;
        u64 multiboot_end;
        u64 kernel_start;
        u64 kernel_end;
    } boundries;

} AreaFrameAllocator;


void choose_next_area(AreaFrameAllocator *allocator) {
    multiboot_memory_map_t *next_area = nullptr;

    for (i32 i = 0; i < allocator->mmap_size; i++) {
        multiboot_memory_map_t *area = &allocator->mmap[i];
        u64 address = area->addr + area->len - 1;

        if (containing_address(address).number >=
            allocator->next_free_frame.number) {
            if (area->addr < next_area->addr) {
                next_area = area;
            }
        }
    }

    allocator->current_area = next_area;

    if (next_area) {
        Frame frame = containing_address(next_area->addr);
        if (allocator->next_free_frame.number < frame.number) {
            allocator->next_free_frame = frame;
        }
    }
}

AreaFrameAllocator new_allocator(MultibootData *multiboot) {
    AreaFrameAllocator allocator;

    allocator.next_free_frame = containing_address(0);
    allocator.current_area = nullptr;
    allocator.mmap = multiboot->mmap.mmap;
    allocator.mmap_size = multiboot->mmap.mmap_size;
    allocator.boundries.multiboot_start =
        multiboot->multiboot_information_start;
    allocator.boundries.multiboot_end = multiboot->multiboot_information_end;
    allocator.boundries.kernel_start = multiboot->kernel_addr.start;
    allocator.boundries.kernel_end = multiboot->kernel_addr.end;

    choose_next_area(&allocator);
    return allocator;
}

typedef enum AllocatorResult {
    Success = 0,
    Error = 1,
} AllocatorResult;

AllocatorResult allocate_frame(AreaFrameAllocator *allocator, Frame *dst) {
    if (allocator->current_area == nullptr) {
        return Error;
    }
    Frame frame = allocator->next_free_frame;

    Frame current_frame = containing_address(allocator->current_area->addr +
                                             allocator->current_area->len - 1);

    if (frame.number > current_frame.number) {
        choose_next_area(allocator);
    } else if (frame.number >= allocator->boundries.kernel_start &&
               frame.number <= allocator->boundries.kernel_end) {
        allocator->next_free_frame.number += 1;
    } else if (frame.number >= allocator->boundries.multiboot_start &&
               frame.number <= allocator->boundries.multiboot_end) {
        allocator->next_free_frame.number += 1;
    } else {
        allocator->next_free_frame.number += 1;
        *dst = frame;
        return Success;
    }

    allocate_frame(allocator, dst);
}
