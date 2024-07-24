; definicja miejsca zaczęcia programu
global  start

section .text

; informacja dla kompilatora o tym że wszystkie poniższe instrukcje
; muszą być zakodowane w systemie 32 bitowym, stało rozmiarowym
bits    32
start:
    ; inicjacja stacku
    ; w procesorach potomnych dla i386 stack zaczyna się od jego góry i kończy na spodzie
    mov esp, stack_top
    
    ; w celu testu procedury sprawdzenia multiboot należy odkomentować poniższą linijkę
    ; mov eax, 0
    ; wywołanie procedury sprawdzenia multiboot
    call multiboot_check

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

; procedura odpowiedzialna za sprawdzenie czy kernel był odpalony z systemu multiboot
; potem będziemy wykorzystywać niektóre własciwości tego systemu
multiboot_check:
    ; z dokumentacji multiboot wiemy że bootloader ładuje na start wartość do rejestru EAX
    ; EAX = 0x36d76289
    ; jeśli w EAX jest ta wartość to korzystamy z multiboot
    ; w przeciwnym razie w EAX będzie 0 ponieważ domyślny start jest na pustych rejestrach
    cmp eax, 0x36d76289
    ; jeśli nie równe, skaczemu do wywołania błędu
    jne .multiboot_error
    ret
    
    ; jeśli procedura nie zadziała poprawnie musimy zgłosić błąd
.multiboot_error:
    mov  al, "0"
    call error



; sekcja danych statycznych
section .bss
; label ułatwiający nam znalezienie spodu stack-u 
stack_bottom:
; zarezerwowanie miejsca na stack
; ta przestrzeń będzie używana tylko i wyłącznie przez część napisaną w ASM
; dla pewności rezerwujemy 64 bajty, wystarczył by 32
    resb 64
; label ułatwiający nam znalezienie góry stack-u
stack_top: