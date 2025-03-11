#pragma GCC diagnostic ignored "-Wwrite-strings"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "graphics/graphics.hpp"
#include "interrupts/interrupts.hpp"
#include "limine.h"
#include "memory/ll_allocator.hpp"
#include "utils/utils.hpp"

__attribute__((used,
               section(".limine_requests_"
                       "start"))) static volatile LIMINE_REQUESTS_START_MARKER;

__attribute__((
    used, section(".limine_requests"))) static volatile LIMINE_BASE_REVISION(3);

__attribute__((
    used,
    section(
        ".limine_requests"))) static volatile struct limine_framebuffer_request
    framebuffer_request = {.id = LIMINE_FRAMEBUFFER_REQUEST, .revision = 0};

__attribute__((
    used,
    section(".limine_requests"))) static volatile struct limine_memmap_request
    memmap_request = {.id = LIMINE_MEMMAP_REQUEST, .revision = 0};

__attribute__((
    used,
    section(".limine_requests"))) static volatile struct limine_hhdm_request
    hhdm_request = {.id = LIMINE_HHDM_REQUEST, .revision = 0};

__attribute__((
    used,
    section(
        ".limine_requests_end"))) static volatile LIMINE_REQUESTS_END_MARKER;

void halt(void) {
    for (;;) {
        asm("hlt");
    }
}

extern "C" void kmain(void) {
    if (LIMINE_BASE_REVISION_SUPPORTED == false) {
        halt();
    }

    if (framebuffer_request.response == nullptr ||
        framebuffer_request.response->framebuffer_count < 1) {
        halt();
    }

    if (memmap_request.response == nullptr) {
        halt();
    }

    if (hhdm_request.response == nullptr) {
        halt();
    }

    struct limine_framebuffer* framebuffer =
        framebuffer_request.response->framebuffers[0];
    struct limine_memmap_response* memmap = memmap_request.response;

    TextMode text_mode(framebuffer);
    idt_init();

    text_mode.printf("This is normal text\n");
    text_mode.error("This is error text\n");
    text_mode.warning("This is warning text\n");
    text_mode.info("This is info text\n");

    text_mode.printf("Resolution: %dx%d\n", framebuffer->width,
                     framebuffer->height);

    text_mode.printf("memmap size: %d\n", memmap->entry_count);
    struct limine_memmap_entry* current_entry = nullptr;
    for (size_t i = 0; i < memmap->entry_count; i++) {
        struct limine_memmap_entry* entry = memmap->entries[i];
        text_mode.printf("\tBase: 0x%x\t|\tLen: %x", entry->base,
                         entry->length);
        if (entry->type == LIMINE_MEMMAP_USABLE && current_entry == nullptr) {
            current_entry = entry;
            text_mode.printf("\tFound new current");
        }
        text_mode.printf("\n");
    }

    LinkedListAllocator ll(
        (void*)(current_entry->base + hhdm_request.response->offset));

    uint32_t loooong_string_size = 1024;
    char* loooong_string = (char*)ll.malloc(loooong_string_size);

    if (loooong_string == nullptr) {
        text_mode.printf("error, nullptr");
    } else {
        for (size_t i = 0; i < loooong_string_size;) {
            for (size_t j = 'a'; i < loooong_string_size && j <= 'z'; j++) {
                loooong_string[i] = (char)j;
                i++;
            }
        }
    }

    text_mode.printf(loooong_string);

    ll.free(ll.malloc(128));
    ll.dbg(text_mode);

    void* reuse = ll.malloc(120);
    ll.dbg(text_mode);

    ll.free(reuse);
    ll.free(loooong_string);

    halt();
}