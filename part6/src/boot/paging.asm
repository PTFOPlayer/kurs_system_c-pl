bits    32


; metoda odpowiedzialna za zapełnienie tablic P2-P4 odpowiednimi adresami
set_up_pageing:
    ; mapowanie P4 do P3
    mov eax, p3_table
    or eax, 0b11 
    mov [p4_table], eax

    ; analogiczny proces dla P3 do P2
    mov eax, p2_table
    or eax, 0b11
    mov [p3_table], eax

    ; mapowanie pól P2 do 2MiB stron pamięci 
    mov ecx, 0         ; counter, takie i w pętli

.map_p2:
    mov eax, 0x200000  
    mul ecx                
    or eax, 0b10000011 
    
    mov [p2_table + ecx * 8], eax

    inc ecx
    cmp ecx, 512       
    jne .map_p2

    ret

; włączenie pagingu
enable_paging:
    mov eax, p4_table
    mov cr3, eax

    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

ret