bits    32

section .multiboot_header
header_start:
    dd 0xe85250d6                

    dd 0                         
    dd header_end - header_start 
    
    dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start))

    dw 0    ; type
    dw 0    ; flags
    dd 8    ; size
header_end:

global start
section .text
start:
    mov esp, stack_top
    mov dword [0xb8000], 0x2f4b2f4f
    hlt



section .bss
align 16
stack_bottom:
    resb 16384 ; 16KiB
stack_top: