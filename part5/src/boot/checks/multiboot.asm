bits    32

multiboot_check:
    cmp eax, 0x36d76289
    jne .multiboot_error
    ret

.multiboot_error:
    mov  al, "M"
    call error