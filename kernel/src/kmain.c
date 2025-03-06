#include "interrupts/interrupts.h"
#include "keyboard/keyboard.h"
#include "vga/colors.h"
#include "vga/vga.h"
#include "serial/serial.h"
#include "serial/serial_dbg.h"
#include "timers/pit.h"
#include "memory/frame_allocator/frame_allocator.h"
// plik multiboot z strony OSDEV
#include "./multiboot/multiboot.h"
#include "./stdlib/assert.h"
#include "./multiboot/multiboot_parse.h"
#include "memory/paging/flags.h"
#include "memory/paging/paging.h"

static u32 pit_tick = 0;
void pit_handle() {
    pit_tick += 1;
    if(pit_tick == 100) {
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
    
    serial_dbg(COM1);
    
    MultibootData data = parse_multiboot(multiboot_info, 1);

    
    AreaFrameAllocator allocator = new_allocator(&data);

    for (i32 i = 0; i < 256; i++)
    {
        Frame f;
        if (!allocate_frame(&allocator, &f))
        {
            printf("Alocation error");
            break;
        }
        
        printf("%d\t", f.number);

        if(i%8 == 0) {
            printf("\n");
        }
    }

    PageTable *page_4 = p4;
    PageTable *page_3 = nullptr;
    // "soft" allocation of page
    set_entry(&page_4->entries[42], containing_address(0xdeadbeef),PAGE_PRESENT);
    assert(next_table(page_4, 42, page_3));
    printf("\n\nSucces\n\n");
    while (1) {
    }
}
