; definicja miejsca zaczęcia programu
global  start

section .text

; informacja dla kompilatora o tym że wszystkie poniższe instrukcje
; muszą być zakodowane w systemie 32 bitowym, stało rozmiarowym
bits    32
start:
    ; wypisanie "OK" na ekranie
    ; adres 0x000b8000 jest na tym etapie adresem danych wypisywanych w trybie tekstowym
    mov dword [0xb8000], 0x2f4b2f4f

    ; hlt - halt
    ; zatrzymuje działanie procesora
    hlt