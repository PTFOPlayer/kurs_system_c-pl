#include "interrupts/interrupts.h"
#include "keyboard/keyboard.h"
#include "vga/colors.h"
#include "vga/vga.h"
#include "serial/serial.h"
#include "serial/serial_dbg.h"
#include "timers/pit.h"

// plik multiboot z strony OSDEV
#include "./multiboot/multiboot.h"
#include "./multiboot/multiboot_parse.h"

static u32 pit_tick = 0;
void pit_handle() {
    pit_tick += 1;
    if(pit_tick == 100) {
        // puts("pit fired");
        pit_tick = 0;
    }
}

void kmain(void *multiboot_info) {
    init_writer(LightGray, Black);
    
    idt_init();
    init_keyboard();
    
    init_memory(multiboot_info, 1);
    

    // init_serial(COM1, 0);
    // serial_dbg(COM1);

    set_pit_handler(pit_handle);
    init_pit(100);

        
    while (1) {
    }
}
