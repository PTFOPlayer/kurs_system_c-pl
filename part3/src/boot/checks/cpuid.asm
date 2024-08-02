; informacja dla kompilatora o tym że wszystkie poniższe instrukcje
; muszą być zakodowane w systemie 32 bitowym, stało rozmiarowym
bits    32

; sprawdzenie czy CPUID jest dostępne poprzez obrrócenie bitu 21 w fejestrze FLAGS
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

    ; ponowne skopiowanie FLAGS do EAX, jeśli bit się obrócił to posiadamy CPUID
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
