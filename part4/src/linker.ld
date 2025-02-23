ENTRY(start)

SECTIONS {
    . = 2M;
    .text :
    {
        KEEP(*(.multiboot_header))
        *(.text .text.*)
    }
    .rodata BLOCK(4K):ALIGN(4K)
    {
        *(.rodata .rodata.*)
    }
    .data BLOCK(4K):ALIGN(4K)
    {
        *(.data .data.*)
    }
    .bss BLOCK(4K):ALIGN(4K)
    {
        *(.bss .bss.*)
    }
    .data.rel.ro : 
    {
        *(.data.rel.ro.local*) *(.data.rel.ro .data.rel.ro.*)
    }
}