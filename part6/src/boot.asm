%include "src/boot/checks/multiboot.asm"
%include "src/boot/checks/cpuid.asm"
%include "src/boot/checks/longmode.asm"
%include "src/boot/paging.asm"

global  start
extern long_mode_start
section .text

bits    32
start:
    ; inicjacja stacku
    mov  esp, stack_top

    ; testy
    call multiboot_check
    call cpuid_check
    call long_mode_check

    call set_up_pageing
    call enable_paging    


    ; przejście do 64 bit
    lgdt [gdt64.pointer] 
    jmp gdt64.code:long_mode_start

    hlt
    
error:
    mov dword [0xb8000], 0x4f524f45
    mov dword [0xb8004], 0x4f3a4f52
    mov dword [0xb8008], 0x4f204f20
    mov byte [0xb800a],  al
    hlt

section .bss
align 4096
; zarezerwowanie miejsca na kolejne PAGE-e do adresowania pamięci
p4_table:
    resb 4096
p3_table:
    resb 4096
p2_table:
    resb 4096


stack_bottom:
    resb 64
stack_top:

section .rodata
gdt64:
    dq 0
.code: equ $ - gdt64
    dq (1<<43) | (1<<44) | (1<<47) | (1<<53)
.pointer:
    dw $ - gdt64 - 1
    dq gdt64