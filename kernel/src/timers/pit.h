#include "../stdlib/io.h"
#include "../stdlib/utils.h"
#include "../stdlib/stdint.h"
#include "../interrupts/interrupts.h"

void set_pit_handler(void(*handler)(IRQFrame)) {
    irq_install(0, handler);
}

void init_pit(u32 freq) {
    u32 divisor = 1193180 / freq; // 119318.166666 Mhz

    out_port(0x43, 0x36);
    out_port(0x40, (u8)(divisor & 0xff));
    out_port(0x40, (u8)(divisor >> 8) & 0xff);
}