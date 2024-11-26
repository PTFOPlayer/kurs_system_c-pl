# Podstawy trybu 32 bit i stos

## Użycie bootloadera Multiboot2
Pierwszą rzeczą jaką zrobimy będzie zdefiniowanie nagłówka multiboot. Jest to konieczne jeśli chcemy korzystać z udogodnień tego bootloadera.

Struktura nagłówka jest prosta, można ją przedstawić w następującej tabeli:

| Pole          | Typ                      | Wartość                                        |
| ------------- | ------------------------ | ---------------------------------------------- |
| magic number  | uint32                   | 0xE85250D6                                     |
| architecture  | uint32                   | 0 dla i386                                     |
| header length | uint32                   | długość headera                                |
| checksum      | uint32                   | -(magic + architecture + header_length)        |
| tags          | nie zdefiniowane         | wartości są różne w zależności od zastosowania |
| end tags      | (uint16, uint16, uint32) | (0,0,8) - tag kończący kernel                  |

Po konwersji na assemblera otrzymujemy następującą kod:
> plik src/boot.asm
```x86asm
section .multiboot_header
header_start:
    dd 0xe85250d6                

    dd 0                         
    dd header_end - header_start 
    
    dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start))

    dw 0
    dw 0
    dd 8
header_end:
```

Pseudo instrukcje `dd` i `dw` możemy rozwinąć, odpowiednio, do `define double word` i `define word`. Słóżą one do definicji pól bitowych w miejscu w którym się znajdują.

Tagi `header_start` oraz `header_end` są przez nas używane do obliczania długości nagłówka. Ponieważ tagi w assemblerze odpowiadają adresom instrukcji znajdujących się zaraz po nich to możemy obliczyć odległość między nimi co da nam długość headera.

## Pierwsze wypisanie na ekran

Programy w assemblerze zawsze znajdują się w sekcji `.text`. Jest to sekcja w której znajduje się kod stricte wykonawczy, to znaczy, nie możemy przechowywać tutaj żadnych danych. W przypadku programowania niskopoziomowego sekcja ta powinna zaczynać się od definicji rozmiaru instrukcji, aktualnie jest to 32 bit. Dopiero nastepnie możemy zdefiniować symbol startowy oraz zacząć pisać program.  

> plik: src/boot.asm
```x86asm
...
header_end:

global start
section .text
bits    32      ; definicja rozmiaru instrukcji
start:
    mov dword [0xb8000], 0x2f4b2f4f
    hlt
```

`global start` definiuje punktem startowy, w naszym kernelu będzie to symbol `start`. Następnie rozpoczynamy sekcję `.text` i definiujemy rozmiar instrukcji.

Teraz w procedurze start możemy zrobić pierwsze wypisanie na ekran.

Na procesorach i386 i ich pochodnych jest to bardzo proste dzięki trybowi VGA text mode. Wewnątrz procesora mamy zapewniony ROM z wszystkimi znakami ascii które może renderować procesor oraz bufor VGA znajdujący sie pod adresem `0xb8000`. Bufor ma miejsce na 2000 wpisów, każdy wpis składa się z znaku ascii, 3 bitowego koloru jego tła oraz 4 bitowego koloru symbolu. A więc jeśli chcemy biały tekst na zielonym tle to bajt koloru to `0x2f`. 

Teraz wypiszmy napis `OK`.
```x86asm
    mov dword [0xb8000], 0x2f4b2f4f
```

Robimy to poprzez przemieszczenie stałej skłądającej się kolejno z bajtu koloru, znaku `'O'`, kolejnego bajtu koloru oraz znaku `'K'` do buforu pod adresem 0xb8000 za pomocą instrukcji `mov`.

## Stos 

Teraz dodajmy do naszego programu stos, będzie on bardzo użyteczny do przechowywania tymczasowych danych w pamięci RAM. Niestety pisząc kod niskopoziomowy nie mamy pod sobą żadnego systemu który mógłby nam przydzielić pamięć która zostałaby użyta na alokację stosu. Zdefiniujemy jego przestrzeń sami w sekcji `.bss`, ta sekcja jest zawsze zerowana na inicjacji. Zarezerwujemy go 16 KiB (Kibibajtów, 16 * 1024 bajty) za pomocą instrukcji `resb`.

> plik: src/boot.asm
```x86asm
...

section .bss
align 16
stack_bottom:
    resb 16384 ; 16KiB
stack_top:
```

Teraz wystarczy załadować wskaźnik do stosu do rejestru `ESP`, ponieważ w i386 i pochodnych stack idzie od góry do dołu to ładujemy wskaźnik do `stack_top`.

```x86asm
mov esp, stack_top
```
