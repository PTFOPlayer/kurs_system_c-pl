#pragma once

#include "../int.h"
#include "colors.h"

const u8 BUFFER_HEIGHT = 25;
const u8 BUFFER_WIDTH = 80;

// definicje funkcji manipulacji tekstu
void kputnl();
void kputs(char *);
void kputc(char);
void kclear();
void kshift_buffer();

// struct z danym znakiem i jego kolorem
// structy w C są sekwencyjne a więc kolejność ma znaczenie
typedef struct VGAChar {
    char ascii_char;
    u8 color;
} VGAChar;

// struct przechowujący informacje o pozycji w tekście
// domyślny kolor oraz pointer do bufora VGA
typedef struct Writer {
    u8 position_width;
    u8 position_height;
    u8 default_color;
    VGAChar *buffer;
} Writer;

// statyczny writer, wszystkie funkcje będą z niego korzystać
//
// jeśli byśmy implementowali wielo wątkowość trzeba dodać pole blokady mutex
// oraz wykonywać operacje atomowo
static Writer writer = {
    .position_width = 0,
    .position_height = 0,
    .default_color = (Black << 4) | LightGray,
    .buffer = (VGAChar *)0xb8000,
};

// czyszczenie
void kclear() {
    writer.position_width = 0;
    writer.position_height = 0;

    VGAChar empty = {.ascii_char = 0, .color = 0};

    for (u8 width = 0; width < BUFFER_WIDTH; width++)
        for (u8 height = 0; height < BUFFER_HEIGHT; height++)
            writer.buffer[height * BUFFER_WIDTH + width] = empty;
}

void kclear_line() {
    VGAChar empty = {.ascii_char = 0, .color = 0};

    for (u8 width = 0; width < BUFFER_WIDTH; width++) {
        writer.buffer[writer.position_height * BUFFER_WIDTH + width] = empty;
    }
}

// wypisywanie charów
void kputc(char ascii) {
    if (ascii == '\n') {
        kputnl();
        return;
    }

    u8 width = writer.position_width;
    u8 height = writer.position_height;

    VGAChar colored_char = {.ascii_char = ascii, .color = writer.default_color};

    writer.buffer[height * BUFFER_WIDTH + width] = colored_char;

    writer.position_width++;    

    if (writer.position_width >= BUFFER_WIDTH) kputnl();
}

// nowa linia
void kputnl() {
    VGAChar empty = {.ascii_char = 0, .color = 0};

    for (u8 width = writer.position_width; width < BUFFER_WIDTH; width++)
        writer.buffer[writer.position_height * BUFFER_WIDTH + width] = empty;

    writer.position_height++;
    writer.position_width = 0;

    if (writer.position_height >= BUFFER_HEIGHT) {
        kshift_buffer();
        writer.position_height--;
        kclear_line();
    }
}

// przesunięcie buffera VGA jeśli dojdzie do końca
void kshift_buffer() {
    for (u8 height = 0; height < BUFFER_HEIGHT - 1; height++)
        for (u8 width = 0; width < BUFFER_WIDTH; width++)
            writer.buffer[height * BUFFER_WIDTH + width] =
                writer.buffer[(height + 1) * BUFFER_WIDTH + width];
}

// wypisanie stringów
void kputs(char *ascii) {
    while (*ascii != '\0') {
        kputc(*ascii);
        ascii++;
    }
}