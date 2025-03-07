#include "interrupts/interrupts.h"
#include "keyboard/keyboard.h"
#include "memory/frame_allocator/frame_allocator.h"
#include "serial/serial.h"
#include "timers/pit.h"
#include "vga/colors.h"
#include "vga/vga.h"
// plik multiboot z strony OSDEV
#include "./multiboot/multiboot.h"
#include "./multiboot/multiboot_parse.h"
#include "./stdlib/assert.h"
#include "memory/paging/flags.h"
#include "memory/paging/paging.h"

static u32 pit_tick = 0;
void pit_handle() {
    pit_tick += 1;
    if (pit_tick == 100) {
        // printf("pit fired");
        pit_tick = 0;
    }
}

void kmain(void *multiboot_info) {
    init_writer(LightGray, Black);
    idt_init();
    init_serial(COM1, 0);
    init_keyboard();

    init_pit(100);
    set_pit_handler(pit_handle);

    MultibootData data = parse_multiboot(multiboot_info, 1);

    AreaFrameAllocator allocator = new_allocator(&data);

    PageTable *p4 = P4_GLOBAL;
    printf("\n\n");
    u64 addr = 42 * 512 * 512 * 4096;
    u64 physical;
    Page p = page_containing_address(addr);
    printf("Addr: %x | Page: %x\n", addr, p);
    Frame f;
    assert(allocate_frame(&allocator, &f));
    printf("Addr: %x | Page: %x | Frame: %x\n", addr, p, f);
    map_to(p, f, 0, &allocator);
    printf("If u see this page has been properly mapped\n");
    assert(translate_address(addr, &physical));
    printf("Addr: %x | Page: %x | Frame: %x | Phys: %x\n", addr, p, f,
           physical);
    
    while (1) {
    }
}
