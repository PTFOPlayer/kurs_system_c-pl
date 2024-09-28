# Testy poprawnego bootowania - Part 2

## Wprowadzenie

Jesteśmy już w trybie protected oraz posiadamy kontrolę nad tym co robi procesor. Mogli byśmy już przechodzić do trybu 64 bitowego, ale procesory mają to do siebie że mają różne specyfikacje, funkcjonalności, oraz zestawy instrukcji.

Z tego powodu musimy napisać odpowiednie testy które pozwolą na poprawne przejście do trybu 64 bit a w przeciwnym razie HALT procesora.

## Struktura projektu

Struktura projektu pozostaje bez zmian względem poprzedniej części. W następnej przeniesiemy check-i do oddzilenych plików

## Inicjacja stosu

Do wykonania niektórych testów będziemy potrzebować stosu, niestety nie mamy pod nami systemu który może nam przydzielić pamięć a więc musimy sami zdecydować ile jej potrzebujemy i zadeklarować tą ilość w sekcji `.bss`

> plik: src/boot.asm
```x86asm
section .bss
stack_bottom:
    resb 64
stack_top:
```

Tagi `stack_bottom` i `stack_top` ułatwiają nam znalezienie granic stosu, na ten moment rezerwujemy tylko 64 bajty, potem będziemy modyfikować tą wartośc.

Teraz musimy zainicjować `ESP` aby móc używać instrukcji służących do wypychania i ściągania danych ze stacku.

> plik: src/boot.asm
```x86asm
...

start:
    ; w procesorach potomnych dla i386 stack zaczyna się od jego góry i kończy na spodzie
    mov  esp, stack_top
    
...
```


## Test Multiboot

Test ten jest odpowiedzialny za sprawdzenie czy system został odpalony z użyciem multiboot (grub), jest to ważne ponieważ potem można wykorzystywać informację o tym do pozyskania dodatkowych informacji o dostępnym sprzęcie.

Z dokumentacji możemy się dowiedzieć że multiboot na zakończenie swojej pracy ładuje do rejestru `EAX` wartość `0x36d76289`, jeśli jest tam inna wartość to nastąpił jakiegoś rodzaju błąd lub nie kernel nie został uruchomiony z użyciem multiboot

> plik: src/boot.asm
```x86asm
multiboot_check:
    cmp eax, 0x36d76289
    
    ; jeśli EAX nie jest równe powyższej wartości to przechodzimy do błędu procesora
    jne .multiboot_error
    ret
 
    ; jeśli procedura nie zadziała poprawnie musimy zgłosić błąd
.multiboot_error:
    mov  al, "M"
    call error
```

## Test CPUID

Sprawdzenie obecności CPUID wykonuje się poprzed obrót bitu 21 w rejestrze FLAGS.
Jeżeli uda nam się poprawnie obrócić bit, wpisać wartość do rejestru FLAGS i pobrać ją ponownie i w trakcie tej operacji bit 21 wróci do swojego początkowego stanu to zestaw instrukcji CPUID jest dostępny.

```x86
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

```

## Test Longmode

Ostatnim krokiem dla nas jest sprawdzenie czy dostępny jest tryb 64 bitowy. Wywołująć instrukcję `cpuid` z argumentem `0x80000000` spowoduje sprawdzenie maksymalnej dostępnej wersji CPUID w naszym procesorze, dla naszych potrzeb wystarczy wersja `0x80000001`.

Instrukcja `Cpuid` z argumentem `0x80000001` ładuje do głównych 4 rejesetrów dużo informacji na temat procesora, w tym informacje o dostępnych trybach działania oraz posiadanych zestawach instrukcji. Po wywołaniu tej instrukcji w rejestrze `EDX` na bicie 29 znajduje się informacja czy longmode jest dostępny.

```x86asm
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
```