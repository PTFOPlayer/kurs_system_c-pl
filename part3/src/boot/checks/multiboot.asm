; informacja dla kompilatora o tym że wszystkie poniższe instrukcje
; muszą być zakodowane w systemie 32 bitowym, stało rozmiarowym
bits    32

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
    mov  al, "M"
    call error