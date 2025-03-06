#pragma once
#include "../../multiboot/multiboot.h"
#include "../../multiboot/multiboot_parse.h"
#include "../../stdlib/stdint.h"
#define PAGE_SIZE 0x1000  // 4096

typedef struct Frame {
    u64 number;
} Frame;

Frame containing_address(u64 address) {
    Frame frame = {address / PAGE_SIZE};
    return frame;
}

u64 frame_start_address(Frame frame) {
    return frame.number * PAGE_SIZE;
}