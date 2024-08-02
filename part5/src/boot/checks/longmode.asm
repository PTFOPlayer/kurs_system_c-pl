bits    32

long_mode_check:
    mov eax, 0x80000000
    cpuid                  
    
    cmp eax, 0x80000001
    jb  .long_mode_error

    mov eax, 0x80000001
    cpuid

    test edx, 1 << 29
    jz   .long_mode_error
    ret
.long_mode_error:
    mov al, "L"
    jmp error