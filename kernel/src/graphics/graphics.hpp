#pragma once
#include <stdarg.h>
#include <stdint.h>

#include "../limine.h"
#include "../utils/utils.hpp"
#include "font.hpp"
class TextMode {
   private:
    uint32_t foreground = TextMode::default_foreground;
    uint32_t background = TextMode::default_background;
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t *addr;
    uint32_t max_width;
    uint32_t max_height;
    void write_char(uint32_t width, uint32_t heigth, uint32_t foreground,
                    uint32_t background, char c);
    void vprintf(char *, va_list);

   public:
    static const uint32_t default_foreground = 0xffffff;
    static const uint32_t default_background = 0x000000;

    static const uint32_t error_foreground = 0xff0000;
    static const uint32_t error_background = 0xffff00;

    static const uint32_t warning_foreground = 0xffff00;
    static const uint32_t warning_background = 0x000000;

    static const uint32_t info_foreground = 0x000000;
    static const uint32_t info_background = 0x00ff00;

    TextMode(struct limine_framebuffer *);

    void set_foreground(uint32_t);
    void set_background(uint32_t);
    uint32_t get_foreground();
    uint32_t get_background();

    void set_default_colors();
    void set_error_colors();
    void set_warning_colors();
    void set_info_colors();

    void shift_buffer();
    void new_ln();
    void clear_line();
    void putc(char);
    void puts(char *);
    void printf(char *, ...);
    void error(char *, ...);
    void warning(char *, ...);
    void info(char *, ...);
    ~TextMode();
};

static TextMode *global_writer = nullptr;
TextMode::TextMode(struct limine_framebuffer *data) {
    global_writer = this;
    this->max_width = data->width;
    this->max_height = data->height;
    this->addr = (uint32_t *)data->address;
}

void TextMode::set_foreground(uint32_t color) { this->foreground = color; }
void TextMode::set_background(uint32_t color) { this->background = color; }

uint32_t TextMode::get_foreground() { return this->foreground; }
uint32_t TextMode::get_background() { return this->background; }

void TextMode::set_default_colors() {
    this->set_background(this->default_background);
    this->set_foreground(this->default_foreground);
}

void TextMode::set_error_colors() {
    this->set_background(this->error_background);
    this->set_foreground(this->error_foreground);
}

void TextMode::set_warning_colors() {
    this->set_background(this->warning_background);
    this->set_foreground(this->warning_foreground);
}

void TextMode::set_info_colors() {
    this->set_background(this->info_background);
    this->set_foreground(this->info_foreground);
}

void TextMode::write_char(uint32_t width, uint32_t heigth, uint32_t foreground,
                          uint32_t background, char c) {
    uint8_t *bitmap = font8x8[(int)c];

    uint32_t offset = heigth * this->max_width + width;

    for (uint32_t i = 0; i < 8; i++) {
        for (uint32_t j = 0; j < 8; j++) {
            if (bitmap[i] & (1 << j)) {
                addr[offset + (i * this->max_width + j)] = foreground;
            } else {
                addr[offset + (i * this->max_width + j)] = background;
            }
        }
    }
}

void TextMode::shift_buffer() {
    for (uint32_t i = 0; i < this->max_height - 8; i++) {
        for (uint32_t j = 0; j < this->max_width; j++) {
            addr[i * this->max_width + j] = addr[(i + 8) * this->max_width + j];
        }
    }
    this->height -= 8;
    clear_line();
}

void TextMode::new_ln() {
    this->height += 8;
    if (this->height >= this->max_height) {
        shift_buffer();
    }
    this->width = 0;
}

void TextMode::clear_line() {
    for (size_t i = 0; i < this->max_width; i += 8) {
        write_char(i, this->height, 0, 0, ' ');
    }
}

void TextMode::putc(char c) {
    switch (c) {
        case '\t':
            putc(' ');
            putc(' ');
            putc(' ');
            putc(' ');
            break;

        case '\n':
        case '\r':
            new_ln();
            break;

        default:
            write_char(this->width, this->height, this->foreground,
                       this->background, c);
            this->width += 8;
            break;
    }
    if (this->width >= this->max_width) {
        this->height += 8;
        this->width = 0;
    }
    if (this->height >= this->max_height) {
        this->shift_buffer();
    }
}

void TextMode::puts(char *c) {
    while (*c != '\0') {
        putc(*c);
        c++;
    }
}

void TextMode::vprintf(char *format, va_list l) {
    char buff[64] = {0};
    while (*format != '\0') {
        char c = *format;
        format++;
        if (c != '%') {
            putc(c);
        } else {
            switch (*format) {
                case 'c':
                    putc((char)va_arg(l, int));
                    break;
                case 's':
                    puts(va_arg(l, char *));
                    break;
                case 'x':
                    memset(buff, '\0', 64);
                    itoa(va_arg(l, int64_t), buff, 16);
                    puts(buff);
                    break;
                case 'b':
                    memset(buff, '\0', 64);
                    itoa(va_arg(l, int64_t), buff, 2);
                    puts(buff);
                    break;
                case 'd':
                    memset(buff, '\0', 64);
                    itoa(va_arg(l, int64_t), buff, 10);
                    puts(buff);
                    break;
                default:
                    puts("unknown: ");
                    putc(*format);
                    break;
            }
            format++;
        }
    }
}

void TextMode::printf(char *format, ...) {
    va_list l;
    va_start(l, format);
    vprintf(format, l);
}

void TextMode::error(char *format, ...) {
    set_error_colors();
    va_list l;
    va_start(l, format);
    vprintf(format, l);
    set_default_colors();
}

void TextMode::warning(char *format, ...) {
    set_warning_colors();
    va_list l;
    va_start(l, format);
    vprintf(format, l);
    set_default_colors();
}

void TextMode::info(char *format, ...) {
    set_info_colors();
    va_list l;
    va_start(l, format);
    vprintf(format, l);
    set_default_colors();
}

TextMode::~TextMode() {}
