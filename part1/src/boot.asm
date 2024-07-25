; definicja miejsca zaczęcia programu
global  start

section .text

; informacja dla kompilatora o tym że wszystkie poniższe instrukcje
; muszą być zakodowane w systemie 32 bitowym, stało rozmiarowym
bits    32
start:
    ; wypisanie "OK" na ekranie
    ; adres 0x000b8000 jest na tym etapie adresem danych wypisywanych w trybie tekstowym
    ; rozbijając wiadomość na częscie: 
    ;   2f - biały znak z zielonym tłem
    ;   4b - O
    ;   2f - ponownie biały znak z zielonym tłęm
    ;   4f - K
    mov dword [0xb8000], 0x2f4b2f4f
    ; hlt - halt
    ; zatrzymuje działanie procesora
    ; używamy zamiast nieskończonej pętli aby wewnętrzne mechanizmy procesora nie powodowały 
    ; wysokiego wytaktowania pojedyńczego rdzenia
    hlt
    
; procedura odpowiedzialna za wypisanie błędu z poziomu ASM
; procedura wypisze "ERR: " i następnie bajt znajdujący się w al
error:
    mov dword [0xb8000], 0x4f524f45
    mov dword [0xb8004], 0x4f3a4f52
    mov dword [0xb8008], 0x4f204f20
    mov byte [0xb800a],  al
    hlt