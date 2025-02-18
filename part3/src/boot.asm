%include "src/boot/boot_tests.asm"
%include "src/boot/gdt.asm"
%include "src/boot/paging.asm"
section .multiboot_header
header_start:
    dd 0xe85250d6                

    dd 0                         
    dd header_end - header_start 
    
    dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start))

    dw 0
    dw 0
    dd 8
header_end:

global start
section .text
bits    32
start:
    mov esp, stack_top
    
    call multiboot_check
    call cpuid_check
    call long_mode_check
    
    call set_up_paging
    call enable_paging

    lgdt [gdt.Pointer]
    jmp gdt.Code:long_mode
    mov dword [0xb8000], 0x2f4b2f4f
    hlt

bits 64

long_mode:
    cli
    mov ax, gdt.Data
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    extern kmain
    call kmain
    mov rax, 0x2f592f412f4b2f4f
    mov qword [0xb8000], rax
    hlt


section .bss
align 16
stack_bottom:
    resb 16384 ; 16KiB
stack_top:
align 4096
p4_table:
    resb 4096
p3_table:
    resb 4096
p2_table:
    resb 4096