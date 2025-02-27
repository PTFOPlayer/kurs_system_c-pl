#pragma once
#include "../interrupts/interrupts.h"
#include "../stdlib/io.h"
#include "../stdlib/stdint.h"

typedef enum COMPort {
    COM1 = 0x3F8,
    COM2 = 0x2F8,
    COM3 = 0x3E8,
    COM4 = 0x2E8,
    COM5 = 0x5F8,
    COM6 = 0x4F8,
    COM7 = 0x5E8,
    COM8 = 0x4E8,
} COMPort;

u8 com_received(COMPort port) { return in_port(COM1 + 5) & 1; }

u8 transmit_empty(COMPort port) {
    return in_port(port + 5) & 0x20;
}

void write_serial(COMPort port, char a) {
    while (transmit_empty(port) == 0);
    out_port(port,a);
}

i8 init_serial(COMPort port, void(*handler)()) {
    // irq off
    out_port(port + 1, 0x00);  // Disable all interrupts

    // dlab (baud rate divisor)
    out_port(port + 3, 0x80);

    // baud 115200 (divisor 1)
    out_port(port + 0, 0x01);
    out_port(port + 1, 0x00);

    // no parity, 8 bits, one stop
    out_port(port + 3, 0x03);

    // fifo 14 byte
    out_port(port + 2, 0xC7);

    // irq
    out_port(port + 4, 0x0B);

    // test serial
    out_port(port + 4, 0x1E);
    out_port(port + 0, 0xAE);

    if (in_port(port + 0) != 0xAE) {
        puts("faulty serial port");        
        return -1;
    }

    out_port(port + 4, 0x0F);

    if (handler != 0 ) {
        if (port == COM1) {
            irq_install(4, handler);
        }
        
        if (port == COM2) {
            irq_install(3, handler);
        }
    }


    set_color(White, Green);
    printf("Serial 0x%x initialized\n", port);
    set_color(LightGray, Black);

    return 0;
}