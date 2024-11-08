; importy plików .ASM
%include "src/boot/checks/multiboot.asm"
%include "src/boot/checks/cpuid.asm"
%include "src/boot/checks/longmode.asm"
%include "src/boot/paging.asm"

; definicja miejsca zaczęcia programu
global  start
extern long_mode_start
section .text

; informacja dla kompilatora o tym że wszystkie poniższe instrukcje
; muszą być zakodowane w systemie 32 bitowym, stało rozmiarowym
bits    32
start:
    ; inicjacja stacku
    mov  esp, stack_top

    ; wywołanie procedury sprawdzenia multiboot
    call multiboot_check
    
    ; wywołanie procedury sprawdzenia CPUID
    call cpuid_check

    ; wywołanie prrocedury sprawdzenia longmode
    call long_mode_check

    ; wywołanie metod odpowiedzialnych za paging
    call set_up_paging
    call enable_paging    


    ; ładowanie pointera do 64 bitowego GDT
    lgdt [gdt64.pointer] 
    ; skok do trybu 64 bit
    jmp gdt64.code:long_mode_start
    
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

; sekcja danych statycznych
section .bss
align 4096
; zarezerwowanie miejsca na kolejne PAGE-e do adresowania pamięci
p4_table:
    resb 4096
p3_table:
    resb 4096
p2_table:
    resb 4096


; label ułatwiający nam znalezienie spodu stack-u 
stack_bottom:
; zarezerwowanie miejsca na stack
; ta przestrzeń będzie używana tylko i wyłącznie przez część napisaną w ASM
; dla pewności rezerwujemy 64 bajty, wystarczył by 32
    resb 64
; label ułatwiający nam znalezienie góry stack-u
stack_top:

; sekcja danych surowych
section .rodata

; nowe GDT
gdt64:
    dq 0        ; null segment
.code: equ $ - gdt64
    dq (1<<43) | (1<<44) | (1<<47) | (1<<53) ; code segment
.pointer:
    dw $ - gdt64 - 1
    dq gdt64