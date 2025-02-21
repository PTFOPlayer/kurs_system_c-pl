global populate_serial_dbg_frame

populate_serial_dbg_frame:
    mov [rdi+112], rax
    mov [rdi+104], rcx
    mov [rdi+96], rdx
    mov [rdi+88], rbx
    mov [rdi+80], rbp
    mov [rdi+72], rsi
    mov [rdi+64], rdi
    mov [rdi+56], r8
    mov [rdi+48], r9
    mov [rdi+40], r10
    mov [rdi+32], r11
    mov [rdi+24], r12
    mov [rdi+16], r13
    mov [rdi+8], r14
    mov [rdi], r15
    ret
