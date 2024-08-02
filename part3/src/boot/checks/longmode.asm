; informacja dla kompilatora o tym że wszystkie poniższe instrukcje
; muszą być zakodowane w systemie 32 bitowym, stało rozmiarowym
bits    32

; sprawdzenie czy procesor wspiera tryb 64 bitowy
long_mode_check:
    ; sprawdzenie czy rozszerzone informacje o pocesorze są dostępne
    mov eax, 0x80000000
    cpuid                  
    ; minimalna wersja z longmode to 0x80000001, jeśli jest mniej to wywołujemy błąd
    cmp eax, 0x80000001
    jb  .long_mode_error

    ; sprawdzenie czy longmode jest dostępny
    ; argument dla CPUID do uzyskania rozszezrzonych informacji 
    mov eax, 0x80000001
    cpuid

    ; sprawdznie czy LM-bit jest ustawiony w EDX, jeśli nie to wywołujemy błąd
    test edx, 1 << 29
    jz   .long_mode_error
    ret
.long_mode_error:
    mov al, "L"
    jmp error