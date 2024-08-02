; informacja dla kompilatora o tym że wszystkie poniższe instrukcje
; muszą być zakodowane w systemie 32 bitowym, stało rozmiarowym
bits    32


; metoda odpowiedzialna za zapełnienie tablic P2-P4 odpowiednimi adresami
set_up_pageing:
    ; mapowanie P4 do P3
    mov eax, p3_table
    ; ustawienie flag OBECNA + ZAPISYWALNA
    or eax, 0b11 
    mov [p4_table], eax

    ; analogiczny proces dla P3 do P2
    mov eax, p2_table
    or eax, 0b11
    mov [p3_table], eax

    ; mapowanie pól P2 do 2MiB stron pamięci 
    mov ecx, 0         ; counter, takie i w pętli

.map_p2:
    ; 2MiB
    mov eax, 0x200000  
    ; adres i-tej strony 
    mul ecx            
    
    ; ustawienie flagOBECNA + ZAPISYWALNA + DUŻA present + writable + huge
    or eax, 0b10000011 
    
    ; wstawienie adresu do P2
    mov [p2_table + ecx * 8], eax

    inc ecx
    ; jeśli ecx == 512 możemy zakończyć mapowanie, mamy już pełne P2 
    cmp ecx, 512       
    jne .map_p2

    ret

; włączenie pagingu
enable_paging:
    ; ładujemy adres P4 do rejestru CR3 przez który procesor sprawdza adresy pamięci
    mov eax, p4_table
    mov cr3, eax

    ; włączenie PAE (Physical Address Extension) w rejestrze CR4
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ; ustawienie bitu LONGMODE w EFER MSR (model specific register)
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    ; włączenie pagingu
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

ret