bits 32

error:
    mov dword [0xb8000], 0x4f524f45
    mov dword [0xb8004], 0x4f3a4f52
    mov dword [0xb8008], 0x4f204f20
    mov byte [0xb800a],  al
    hlt

multiboot_check:
    cmp eax, 0x36d76289
    jne .multiboot_error
    ret
.multiboot_error:
    mov  al, "M"
    call error


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
