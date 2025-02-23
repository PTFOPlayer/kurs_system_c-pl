#pragma once
#include "../stdlib/stdint.h"
#include "../stdlib/string.h"
#include "colors.h"

void init_writer(byte foreground_color, byte background_color);
void set_color(byte foreground_color, byte background_color);
void clear();
void clear_line();
void shift_buffer();
void putnl();
void putc(char);
void puts(char*);

const u32 BUFFER_HEIGTH = 25;
const u32 BUFFER_WIDTH = 80;
const u32 BUFFER_SIZE = BUFFER_HEIGTH * BUFFER_WIDTH;

typedef struct VGAChar {
    char ascii;
    byte color;
} VGAChar;

typedef struct Writer {
    VGAChar* buffer;
    u32 position;
    byte current_color;
} Writer;

static Writer writer = {.buffer = (VGAChar*)(0xb8000),
                        .position = 0,
                        .current_color = LightGray | (Black << 4)};

void init_writer(byte foreground_color, byte background_color) {
    writer.buffer = (VGAChar*)(0xb8000);
    memset(0, writer.buffer, sizeof(VGAChar) * BUFFER_SIZE);
    writer.position = 0;
    writer.current_color = foreground_color | (background_color << 4);
}

void set_color(byte foreground_color, byte background_color) {
    writer.current_color = foreground_color | (background_color << 4);
}

void clear() { memset(0, writer.buffer, sizeof(VGAChar) * BUFFER_SIZE); }

void clear_line() {
    u32 line_start = (writer.position / BUFFER_WIDTH) * BUFFER_WIDTH;
    VGAChar* positioned_buffer = writer.buffer + line_start;
    memset(0, positioned_buffer, sizeof(VGAChar) * BUFFER_WIDTH);

    writer.position = line_start;
}

void putnl() {
    u32 line_start = (writer.position / BUFFER_WIDTH) * BUFFER_WIDTH;
    u32 next_line_start = line_start + BUFFER_WIDTH;
    VGAChar* positioned_buffer = writer.buffer + writer.position;
    memset(0, positioned_buffer,
           sizeof(VGAChar) * (next_line_start - writer.position));

    writer.position = next_line_start;
    if (writer.position >= BUFFER_SIZE) {
        shift_buffer();
        writer.position = BUFFER_SIZE - BUFFER_WIDTH;
    }
}

void shift_buffer() {
    for (u32 i = 0; i < BUFFER_HEIGTH; i++)
        memcpy(writer.buffer + ((i + 1) * BUFFER_WIDTH),
               writer.buffer + (i * BUFFER_WIDTH),
               sizeof(VGAChar) * BUFFER_WIDTH);
}

void putc(char ascii) {
    switch (ascii) {
        case '\n':
            putnl();
            break;
        case '\t':
            putc(' ');
            putc(' ');
            putc(' ');
            putc(' ');
            break;
        default:
            VGAChar chr = {
                .ascii = ascii,
                .color = writer.current_color,
            };
            writer.buffer[writer.position] = chr;
            writer.position++;
            break;
    }

    if (writer.position >= BUFFER_SIZE) {
        shift_buffer();
        writer.position = BUFFER_SIZE - BUFFER_WIDTH;
    }
}

void puts(char* ascii) {
    while (*ascii != '\0') {
        putc(*ascii);
        ascii++;
    }
}