#pragma GCC diagnostic ignored "-Wwrite-strings"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "acpi/rsdt/rsdt.hpp"
#include "acpi/rsdt/rsdt_wrapper.hpp"
#include "graphics/graphics.hpp"
#include "interrupts/interrupts.hpp"
#include "interrupts/pit.hpp"
#include "keyboard/keyboard.hpp"
#include "limine.h"
#include "memory/hhdm_offset.hpp"
#include "memory/ll_allocator.hpp"
#include "memory/operators.hpp"
#include "pci/pci.hpp"
#include "tasks/context.hpp"
#include "utils/buffers/circular_buffer.hpp"
#include "utils/utils.hpp"

__attribute__((used,
               section(".limine_requests_"
                       "start"))) static volatile LIMINE_REQUESTS_START_MARKER;

__attribute__((
    used, section(".limine_requests"))) static volatile LIMINE_BASE_REVISION(0);

__attribute__((
    used,
    section(
        ".limine_requests"))) static volatile struct limine_framebuffer_request
    framebuffer_request = {
        .id = LIMINE_FRAMEBUFFER_REQUEST, .revision = 0, .response = nullptr};

__attribute__((
    used,
    section(".limine_requests"))) static volatile struct limine_rsdp_request
    rsdp_request = {
        .id = LIMINE_RSDP_REQUEST, .revision = 0, .response = nullptr};

__attribute__((
    used,
    section(".limine_requests"))) static volatile struct limine_memmap_request
    memmap_request = {
        .id = LIMINE_MEMMAP_REQUEST, .revision = 0, .response = nullptr};

__attribute__((
    used,
    section(".limine_requests"))) static volatile struct limine_hhdm_request
    hhdm_request = {
        .id = LIMINE_HHDM_REQUEST, .revision = 0, .response = nullptr};

__attribute__((
    used,
    section(
        ".limine_requests_end"))) static volatile LIMINE_REQUESTS_END_MARKER;

void halt();
void check_protocol();

extern "C" void kmain(void) {
    check_protocol();

    uint64_t hhdm_offset = hhdm_request.response->offset;
    limine_framebuffer* framebuffer =
        framebuffer_request.response->framebuffers[0];

    TextMode text_mode(framebuffer);
    info("Resolution: %dx%d\n", framebuffer->width, framebuffer->height);

    limine_memmap_response* memmap = memmap_request.response;
    limine_memmap_entry* first = find_first_valid_mmap(memmap);
    LinkedListAllocator ll(get_base(first->base, hhdm_offset));
    check_pci();
    idt_init();
    keyboard_init();

    RSDPWrapper rsdp_wrapped = RSDPWrapper(rsdp_request.response->address);

    if (!rsdp_wrapped.is_xsdp()) {
        error("Expected xsdp");
        halt();
    }

    XSDPHandle xsdp = rsdp_wrapped.get_xsdp();
    xsdp.print();

    pit_init(10);
    set_pit_handler([](IRQFrame* frame) {});

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

    if (rsdp_request.response == nullptr ||
        rsdp_request.response->address == nullptr) {
        halt();
    }
};