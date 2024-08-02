bits    32

cpuid_check:
    pushfd
    pop eax

    mov ecx, eax

    xor eax, 1 << 21

    push eax
    popfd

    pushfd
    pop eax

    push ecx
    popfd

    cmp eax, ecx
    je  .cpuid_error
    ret
.cpuid_error:
    mov al, "C"
    jmp error
