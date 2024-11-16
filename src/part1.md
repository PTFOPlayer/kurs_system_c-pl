# Minimalny kernel - Part 1

## Wprowadzenie

W momencie uruchomienia komputera BIOS wykonuje self test i rutyny inicjalizacyjne. Następnym krokiem jest przekazanie kontroli do bootloadera. Bootloader zajmuje się przejściem procesora w `32 bit protected mode` (ponieważ procesor bootuje w `16 bit real mode`) a następnie znajduje kernel na nośniku urządzenia i ładuje go do pamięci .

My nie będziemy pisać bootloadera, jest to zbyt skomplikowane i czasochłonne, zamiast tego użyjemy `GRUB2`.

## Struktura projektu 

```
|---Makefile
|---build
|   | ...
|---src
    |---boot.asm
    |---multiboot_header.asm
    |---linker.ld
    |---grub.cfg
```

## Multiboot

Na szczęście istnieje standardowy bootloader: [multiboot](https://en.wikipedia.org/wiki/Multiboot_specification).

Aby użyć bootloader-a `multiboot` musimy zacząć pisanie naszego kernela od nagłówka składającego się z następujących pól

| Pole          | Typ                      | Wartość                                        |
| ------------- | ------------------------ | ---------------------------------------------- |
| magic number  | uint32                   | 0xE85250D6                                     |
| architecture  | uint32                   | 0 dla i386                                     |
| header length | uint32                   | długość headera                                |
| checksum      | uint32                   | -(magic + architecture + header_length)        |
| tags          | nie zdefiniowane         | wartości są różne w zależności od zastosowania |
| end tags      | (uint16, uint16, uint32) | (0,0,8) - tag kończący kernel                  |

Konwertując do assembly x86:
> plik: src/multiboot_header.asm
 
```x86asm
section .multiboot_header
header_start:
    ; liczba magiczna
    dd 0xe85250d6                
    ; architektura 0 - tryb protected i386
    dd 0                         
    ; długość headera - używana przez multiboot do sprawdzenia sumy kontrolnej
    dd header_end - header_start 
    
    ; suma kontrolna - informuje bootloader o poprawnym działaniu
    dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start))

    ; opcjonalne tagi (inocjacja FPU lub SSE)

    ; tagi końcowe
    dw 0    ; type
    dw 0    ; flags
    dd 8    ; size
header_end:
```

* header_start i header_end - są to tagi (label) w pliku dzięki którym assembler będzie za nas określał długość headera
* dd - 'define double' określa dowolną zmienną 32 bitową która będzie bezpośrednio wkompilowana w binarkę 
* dw - 'define word' to samo co dd tylko dla 16 bitów

## Boot

Aby nasz kernel mógł wystartować musi istnieć jakiś kod który będzie mógł być wywołany przez multiboot

> plik: src/boot.asm

```x86asm
; definicja miejsca zaczęcia programu
global  start

section .text

; informacja dla kompilatora o tym że wszystkie poniższe instrukcje
; muszą być zakodowane w systemie 32 bitowym, stało rozmiarowym
bits    32
start:
    ; wypisanie "OK" na ekranie
    mov dword [0xb8000], 0x2f4b2f4f

    ; hlt - halt
    ; zatrzymuje działanie procesora
    hlt
```

Wypisanie "OK" - wypisanie czegokolwiek na tym etapie polega na manualnym wpisywaniu danych do buforu VGA, docelowo napiszemy do tego minimalny driver monolityczny (wbudowany w jądro).  

Instrukcja `mov dword [addr], data` powoduje wpisanie danych o rozmiarze dword (32 bit) do podanego adresu. W tym przypadkiem adresem 'addr' jest 0xb8000, czyli adres buforu VGA, nastomiast dane to zakodowany w odpowiedni sposób tekst wraz z kolorami. Jeden znak w tekście defuniuje się jako 4 bity koloru tła, 4 bity koloru tekstu, 8 bitów znaku ASCII. Powyższy tekst jest zakodowany w następujący sposób: 
* 2f - biały znak z zielonym tłem
* 4b - O
* 2f - ponownie biały znak z zielonym tłęm
* 4f - K

## Kompilacja 

W celu automatyzacji kompilacji przygotowałem następujący [Makefile](https://github.com/PTFOPlayer/kurs_system_c-pl/blob/main/part1/Makefile) w który nie będę się zagłębiał.

Natomiast zwrócę uwagę na pliki `linker.ld` oraz krótko do czego służy `grub.cfg`

> plik: src/linker.ld
```ld

ENTRY(start)

SECTIONS {
    . = 1M;

    .boot :
    {
        /* 
            umieszczenie sekcji multiboot na początku programu
            z tego momentu zaczyna się działanie systemu
        */
        *(.multiboot_header)
    }

    .text :
    {
        /*
            główna sekcja programu
        */
        *(.text)
    }
}
```

Plik linker.ld definiuje nam w jakie miejsca jest wstawiana która część programu. Na poczatku mamy `. = 1M;` jest to ignorowanie pierwszego MiB miejsca na nośniku, ponieważ znajdują się tam specjalne adresy. Następnie mamy sekcję `.boot` w której będzie informacja o standardzie multiboot a dopiero na końcu przechodzimy do właściwego kernela w sekcji `.text`.

>plik: src/grub.cfg
```
set timeout=0
set default=0

menuentry "os" {
    multiboot2 /boot/kernel.bin
    boot
}
```

Ten plik definiuje nam w jaki sposób ma być stworzony plik ISO naszego systemu. `menuentry` oznacza nazwę która będzie się pojawiać w tabeli grub2, następnie w klamerkach definiujemy ścieżkę do binarki kernela oraz w następnej linijce nazwę punktu wejścia.

> przydatne informacje na temat użycia grub-a w tego typu projektach: https://wiki.osdev.org/GRUB
