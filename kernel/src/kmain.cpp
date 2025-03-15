#pragma GCC diagnostic ignored "-Wwrite-strings"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "graphics/graphics.hpp"
#include "interrupts/interrupts.hpp"
#include "interrupts/pit.hpp"
#include "keyboard/keyboard.hpp"
#include "limine.h"
#include "memory/ll_allocator.hpp"
#include "utils/utils.hpp"
#include "utils/buffers/circular_buffer.hpp"
#include "pci/pci.hpp"

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

__attribute__((used,
               section(".requests"))) static volatile struct limine_smp_request
    smp_request = {.id = LIMINE_SMP_REQUEST, .revision = 1};

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

void halt();
void check_protocol();

extern "C" void kmain(void) {
    check_protocol();

    limine_framebuffer* framebuffer =
        framebuffer_request.response->framebuffers[0];
    limine_memmap_response* memmap = memmap_request.response;

    TextMode text_mode(framebuffer);
    idt_init();
    keyboard_init();
    pit_init(100);
    set_pit_handler([](IRQFrame frame) {
        // pit processes
    });

    info("Resolution: %dx%d\n", framebuffer->width, framebuffer->height);

    limine_memmap_entry* first = find_first_valid_mmap(memmap);
    LinkedListAllocator ll(get_base(first, hhdm_request.response));
    
    check_pci();
    halt();
}

void halt(void) {
    for (;;) {
        asm("hlt");
    }
}

void check_protocol() {
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
};