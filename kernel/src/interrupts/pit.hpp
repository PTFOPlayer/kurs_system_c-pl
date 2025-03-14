#include "../utils/utils.hpp"
#include "interrupts.hpp"

void set_pit_handler(void (*handler)(IRQFrame)) { irq_install(0, handler); }

void pit_init(uint32_t freq) {
    uint32_t divisor = 1193180 / freq;  // 119318.166666 Mhz

    out_b(0x43, 0x36);
    out_b(0x40, (uint8_t)(divisor & 0xff));
    out_b(0x40, (uint8_t)(divisor >> 8) & 0xff);

    info("PIT initiated\n");
}