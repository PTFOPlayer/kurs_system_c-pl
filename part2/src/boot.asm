; definicja miejsca zaczęcia programu
global  start

section .text

; informacja dla kompilatora o tym że wszystkie poniższe instrukcje
; muszą być zakodowane w systemie 32 bitowym, stało rozmiarowym
bits    32
start:
    ; inicjacja stacku
    ; w procesorach potomnych dla i386 stack zaczyna się od jego góry i kończy na spodzie
    mov  esp, stack_top
    
    ; w celu testu procedury sprawdzenia multiboot należy odkomentować poniższą linijkę
    ; mov eax, 0
    ; wywołanie procedury sprawdzenia multiboot
    call multiboot_check
    
    ; wywołanie procedury sprawdzenia CPUID
    call cpuid_check

    ; wywołanie prrocedury sprawdzenia longmode
    call long_mode_check

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

multiboot_check:
    cmp eax, 0x36d76289

    ; jeśli nie równe, skaczemu do wywołania błędu
    jne .multiboot_error
    ret
    
    ; jeśli procedura nie zadziała poprawnie musimy zgłosić błąd
.multiboot_error:
    mov  al, "M"
    call error


; sprawdzenie czy CPUID jest dostępne poprzez obrócenie bitu 21 w rejestrze FLAGS
cpuid_check:
    ; skopiowane FLAGS do EAX używając stack-u
    ; pushfd wypycha FLAGS na stack
    pushfd
    pop eax

    ; kopiowanie FLAGS z EAX do ECX
    mov ecx, eax

    ; obrócenie bitu 21
    xor eax, 1 << 21

    ; ustawienie rejestru FLAGS na wartość z EAX z obróconym bitem 21
    push eax
    popfd

    ; ponowne skopiowanie FLAGS do EAX, jeśli bit się obrócił z powrotem to posiadamy CPUID
    pushfd
    pop eax

    ; przywrócenie początkowej wersji rejestru FLAGS
    push ecx
    popfd

    ; jeżeli EAX i ECX są równe to znaczy że bit 21 nie został obrucony co znaczy że nie posiadamy CPUID
    cmp eax, ecx
    je  .cpuid_error
    ret
.cpuid_error:
    mov al, "C"
    jmp error


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