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
#include "memory/operators.hpp"
#include "pci/pci.hpp"
#include "tasks/context.hpp"
#include "utils/buffers/circular_buffer.hpp"
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
    framebuffer_request = {
        .id = LIMINE_FRAMEBUFFER_REQUEST, .revision = 0, .response = nullptr};

__attribute__((
    used,
    section(".limine_requests"))) static volatile struct limine_smp_request
    smp_request = {
        .id = LIMINE_SMP_REQUEST, .revision = 1, .response = nullptr};

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

void ctx1_fn() {
    while (1) {
    }
}

void ctx2_fn() {
    while (1)
    {
    }
    
}

static Context* ctx1 = nullptr;
static Context* ctx2 = nullptr;
static int ctx = 3;

extern "C" void kmain(void) {
    check_protocol();

    limine_framebuffer* framebuffer =
        framebuffer_request.response->framebuffers[0];
    limine_memmap_response* memmap = memmap_request.response;

    TextMode text_mode(framebuffer);
    idt_init();
    keyboard_init();
    info("Resolution: %dx%d\n", framebuffer->width, framebuffer->height);
    limine_memmap_entry* first = find_first_valid_mmap(memmap);
    LinkedListAllocator ll(get_base(first, hhdm_request.response));
    check_pci();

    pit_init(10);

    ctx1 = new Context(ctx1_fn);

    ctx2 = new Context(ctx2_fn);

    set_pit_handler([](IRQFrame* frame) {
        if (!ctx1 || !ctx2) {
            return;
        }

        // print_frame(frame);
        if (ctx == 3) {
            printf("opt rsp: %x, rsp: %x\n", frame->optional_rsp, frame->stack);

            ctx1->prepare_from_frame(frame);
            ctx2->prepare_from_frame(frame);
            ctx = 0;
        } else if (ctx == 0) {
            ctx2->copy_frame(frame);
            ctx1->set_frame(frame);
            ctx1->restore_return(frame);
            ctx = 1;
        } else {
            ctx1->copy_frame(frame);
            ctx2->set_frame(frame);
            ctx2->restore_return(frame);
            ctx = 0;
        }
    });

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