#pragma once
#include "../stdlib/stdint.h"
#include "../stdlib/stdlib.h"

const u32 BUFFER_HEIGTH = 25;
const u32 BUFFER_WIDTH = 80;
const u32 BUFFER_SIZE = BUFFER_HEIGTH*BUFFER_WIDTH;

typedef struct VGAChar {
    char ascii;
    byte color;
} VGAChar;

typedef struct Writer {
    VGAChar* buffer;
    u32 position;
    byte current_color;
} Wrtier;

static Writer writer;

Writer* init_writer(byte foreground_color, byte background_color) {
    writer.buffer = (VGAChar*)(0xb8000);
    memset(0, writer.buffer, sizeof(VGAChar)*BUFFER_SIZE);
    writer.position = 0;
    writer.current_color = foreground_color | (background_color << 4);

    return &writer;
}

