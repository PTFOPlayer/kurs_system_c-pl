#include "serial.h"

struct DBGFrame {
    u64 r15, r14, r13, r12, r11, r10, r9, r8, rdi, rsi, rsp, rbp, rdx, rcx, rbx,
        rax;
};

extern void populate_serial_dbg_frame(struct DBGFrame*);

void serial_dbg_register(char* reg, u64 var, char* buff, COMPort port) {
    for (i32 i = 0; i < 16; i++) buff[i] = '0';
    itoa(var, buff, 16);
    
    while (*reg != 0)
    {
        write_serial(port, *reg);
        reg += 1;
    }
    write_serial(port, ':');
    
    for (i32 i = 0; i < 16; i++)
        write_serial(port, buff[i]);   
}

void serial_dbg_register_pair(char* reg1, char* reg2, u64 var1, u64 var2, char* buff, COMPort port) {
    dbg_register(reg1, var1, buff);
    for (i32 i = 0; i < 4; i++)
    write_serial(port, ' ');

    dbg_register(reg2, var2, buff);
    write_serial(port, '\n');
}

void serial_dbg(COMPort port) {
    struct DBGFrame frame = {0};
    populate_serial_dbg_frame(&frame);

    char buff[16] = {0};

    serial_dbg_register_pair("rax", "rbx", frame.rax, frame.rbx, buff, port);
    serial_dbg_register_pair("rcx", "rdx", frame.rcx, frame.rdx, buff, port);
    serial_dbg_register_pair("rbp", "rsp", frame.rbp, frame.rsp, buff, port);
    serial_dbg_register_pair("rsi", "rdi", frame.rsi, frame.rdi, buff, port);
    serial_dbg_register_pair("r8 ", "r9 ", frame.r8, frame.r9, buff, port);
    serial_dbg_register_pair("r10", "r11", frame.r10, frame.r11, buff, port);
    serial_dbg_register_pair("r12", "r13", frame.r12, frame.r13, buff, port);
    serial_dbg_register_pair("r14", "r15", frame.r14, frame.r15, buff, port);
}