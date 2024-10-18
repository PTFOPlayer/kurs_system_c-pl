# Paging - Part 3

## Zmiany w projekcie

Tą część zaczniemy inaczej bo od zmian które następują w projecie. 

Nowa struktura projektu:
```
|---Makefile
|---build
|   | ...
|---src
    |---boot
    |   |---checks
    |   |   |---cpuid.asm
    |   |   |---longmode.asm
    |   |   |---multiboot.asm
    |   |---paging.asm
    |---boot.asm
    |---multiboot_header.asm
    |---linker.ld
    |---grub.cfg
```

Jak widzimy pojawiło nam sie łacznie 4 nowe pliki:
- cpuid.asm
- longmode.asm
- multiboot.asm
- paging.asm

Plik `paging.asm` zostanie wyjaśniony później a na ten moment zajmijmy się pierwszymi trzema plikami. Do tych plików zostały przeniesione testy które wcześniej znajdowały się w pliku boot.asm w celu lekkiego uprzątnięcia kodu. Teraz trzeba te pliki importować w następujący sposób:

> plik: boot.asm
```x86asm
%include "src/boot/checks/multiboot.asm"
%include "src/boot/checks/cpuid.asm"
%include "src/boot/checks/longmode.asm"
```

Ważne jest również aby wszystkie te pliki zaczynały się od `bits 32` jako że jestesmy nadal w trybie 32 bitowym.

## Wprowadzenie

Paging, jest to metoda zarządzania pamięcią która oddziela pamięć wirtualną od pamięci fizycznej. Przestrzeń adresowa jest podzielona na równego rozmiaru "strony" i `page table` definiuje który adres wirtualny którego któremu  adresowi fizycznemu.

W `x86_64` rozmair strony to 4096 bajtów i 4 poziomiwe "strony", każda ze "stron" posiada 512, 8 bajtowych sektorów. 

![image](./photos/paging.png)

Strony:
- Page-Map Level-4 Table (PML4),
- Page-Directory Pointer Table (PDP),
- Page-Directory Table (PD),
- the Page Table (PT).

Mi się te nazwy nie podobają więc będziemy używać odpowiednio nazw od P4..P1 lub L4..L1.

## Struktura sektoru strony

| Bity  | Nazwa                 | Opis                                                              |
| ----- | --------------------- | ----------------------------------------------------------------- |
| 0     | present               | strona znajduje się w pamięci                                     |
| 1     | writable              | można zapisać do strony                                           |
| 2     | user accesible        | dostępna na poziomie urzytkownika                                 |
| 3     | write through caching | bezpośredni zapis                                                 |
| 4     | no cache              | nie urzywaj pamięci cache                                         |
| 5     | accesed               | CPU ustawia ten bit kiedy strona jest w użyciu                    |
| 6     | dirty                 | CPU ustawia ten bit kiedy strona jest w trakcie zapisu            |
| 7     | huge page/null        | strona rozszerzona do 2MB w Page Level 2 i do 1 GB w Page Level 3 |
| 8     | global                | strona nie jest zapisywana jeśli znajduje się w pamięci cache     |
| 9-11  | available             | dostępne dla systemu                                              |
| 12-52 | -                     | pominięte, ze względu na unikalnośc adresów                       |
| 52-62 | available             | dostępne dla systemu                                              |
| 63    | no execute            | zakaz wykonywania kodu z tej strony (bit NXE)                     |

## Ograniczenia

Używając bitu `huge page` na stronach poziomu trzeciego moglibyśmy mapować pierwszy 1GB naszego kernela do pojedyńczej strony co bo znacznie poprawiło jego responsywność i pozwoliło procesorowi na ustalenie lepszych zasad cache-owania. Niestety nie możemy tego zrobić ponieważ `huge page` na L3 został wprowadzony dopiero w generacji `Westmere` (seria Xeon), `Haswell` (seria Core I 4 gen), a więc aby zagwarantować kompatybilnośc z starszymi procesorami i maszynami wirtualnymi będziemy stosować bit `huge page` na stronach L2

Strony są przeszukiwane idąc od L4 do L1, przez co najpierw jest przeszukiwana najdawniej alokowana pamięć.

Paging nie oznacza możliwości alokacji na heap. Do tego trzeba dalej napisać alokator (nie będziemy tego robić).

Paging nie jest zawsze taki sam, w przypadku bardzo dużej ilości pamięci inaczej się rozkłada strony. Są również bardziej zaawansowane metody translacji adresów.

Paging jest ściśle wymagany przez tryb 64 bitowy, bez niego procesor nie wejdzie w ten tryb i wykona przerwanie które kończy się instrukcją `halt`.

## Ktoś może zapytać, po co nam paging, co umożliwia?
Paging jest konieczny do wejścia do trybu 64 bitowego który jest... tak naprawdę 48 bitowy jeśli patrzymy na adresowanie. Maksymalna ilość adresów bezpośrednio w TLB to 2^48 czyli *teoretycznie* możliwe jest adresowanie 32 TB pamięci RAM. 

Ale czemu *teoretycznie*? -> Musimy jeszcze wziąć pod uwagę pamięć cache oraz pamięć `SWAP` lub architekturę NUMA (jej nie będziemy implementować aby wszystko uprościć). 

Te typy pamięci są bardzo ważne z różnych powodów w które nie chcę się zagłębiać ale żeby poprawnie działały musi istnieć paging który będzie nam tłumaczył adresy fizyczne tych obszarów na adresy wirtualne które będą możliwe do użycia przez procesor i programy które wykonuje. 

## Implementacja paging-u

W pierwszym kroku musimy na nasze page-e stworzyć obszar na stacku. Najłatwiej będzie to zrobić modyfikując obszar danych statycznych `.bss`

> plik: boot.asm
```x86asm
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

stack_bottom:
    resb 64
stack_top:
``` 

Pojawiły nam tu się 3 strony pamięci p2, p3 oraz p4. Słowo `align` jest tutaj dosyć ważne jako że chcemy aby te tabele były równej wielkości i zajmowały takie same obszary pamięci (gdyby nie obecność stack-u nie było by to konieczne). Teraz wystarczy je odpowiednio mapować, adresy tabeli p4 są już gotowe jako że GRUB initializuje sekcje `.bss` do zer, zostaje więc mapowanie p4->p3 i p3->p2.

> plik: src/boot/paging.asm
```x86asm
bits    32

; metoda odpowiedzialna za zapełnienie tablic P2-P4 odpowiednimi adresami
set_up_pageing:
    ; mapowanie P4 do P3
    mov eax, p3_table
    or eax, 0b11 
    mov [p4_table], eax

    ; analogiczny proces dla P3 do P2
    mov eax, p2_table
    or eax, 0b11
    mov [p3_table], eax

    ; mapowanie pól P2 do 2MiB stron pamięci 
    mov ecx, 0         ; counter, takie i w pętli

.map_p2:
    mov eax, 0x200000  
    mul ecx                
    or eax, 0b10000011 
    
    mov [p2_table + ecx * 8], eax

    inc ecx
    cmp ecx, 512       
    jne .map_p2

    ret
```

Trochę to długie ale najłatwiej będzie wytłumaczyć to za jednym razem.

Pierwsze dwa mapowania są dosyć proste, bierzemy adres wyższej tabeli za pomocą `mov`:
```x86asm
mov eax, p3_table
``` 
Następnie nadajemy bity `present` i `writable` wyższym stronom za pomocą instrukcji `or`:
```x86asm
or eax, 0b11
```
I ostatecznie ustawiamy pierwszy adres niższej strony na adres wyższej z ustawionymi bitami:
```x86asm
mov [p4_table], eax
```

Trochę bardziej się sprawa komplikuje w `p2_table` jak chcemy używać stron po 2MiB. Jest to również ostatnia strona a więc musimy mapować wszystkie adresy. W tym celu użyjemy pętli.

```x86asm
    ; mapowanie pól P2 do 2MiB stron pamięci 
    mov ecx, 0         ; counter, takie i w pętli

.map_p2:
    mov eax, 0x200000  
    mul ecx            ; eax zawiera teraz adres ecx * 0x200000
    or eax, 0b10000011 
    
    mov [p2_table + ecx * 8], eax

    inc ecx
    cmp ecx, 512       
    jne .map_p2

    ret
```

Pętla mapuje tutaj wszystkie adresy w P2 do oddzielnych bloków po 2MiB. W każdej kolejnej stronie ustawiamy tym razem 3 flagi: `present`, `writable` i `huge`, ponieważ procesor musi wiedzieć że te strony są większe niż standardowe.

Teraz można przejść do włączenia paging-u

> plik: src/boot/paging.asm
```x86asm

; włączenie paging-u
enable_paging:
    ; załadowanie P4 do rejestru CR3 (CPU sprawdza tutaj adres P4)
    mov eax, p4_table
    mov cr3, eax

    ; włączenie flagi PAE (physical addres extension)
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ; włączenie trybu long mode w rejestrze EFER MSR
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    ; włączenie paging-u w CR0
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

ret
```

Patrząc na ten kod pewnie nikt nic nie zrozumie bez wiedzy o tym czym są rejestry `CR`. Rejestry `CR` są grupą rejestrów kontrolnych procesora za pomocą których możemy definiować takie rzeczy jak dostępne rozszerzenia procesora, dostępne akceleratory, sposoby zarządzania pamiecią, włączenie i wyłączenie przerwań oraz zaweirają one informacje o niektórych prostrzych błędach. W tym wypadku wykorzystamy parę z nich do włączenia paging-u.

> informacje o rejestrach CR: https://wiki.osdev.org/CPU_Registers_x86#Extended_Control_Registers

A więc idąc od góry.
```x86asm
    ; załadowanie P4 do rejestru CR3 (CPU sprawdza tutaj adres P4)
    mov eax, p4_table
    mov cr3, eax
```
Pierwsza instrukcja ładuje nam adres `p4_table` do rejestru `eax` a druga z rejestru `eax` do rejestru `CR3` (rejestr adresu pierwszej strony). 

A czemu nie można odrazu załadować adresu strony do rejestru `CR3`? Na przykład pisząc `move cr3, p4_table`?

Niesty ale rejestry `CRx` są ograniczone. Zapis do nich musi odbywać się z któregoś z rejestrów ogólnego przeznaczenia, oraz nie podlegają żadnym instrukcjom oprócz instrukcji `mov`. Jest to zrobione aby je zabezpieczyć przed przypadkowym zapisem.

```
    ; włączenie flagi PAE (physical addres extension)
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ; włączenie trybu long mode w rejestrze EFER MSR
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr
```

Te dwa segmenty odpowiadają za ustawienie odpowiednich flag w kolejnych rejestrach procesora. Tym razem spotykamy się z rejestrami `msr` których jest kilka tysięcy więc pozwolę sobie ich nie omawiać szczegółowo, natomiast uogólniając są to rejestry z których możemy dowiedzieć się wszystkich możliwych rzeczy o procesorze. Na przykład jego nazwę, ilość pamięci cache, liczniki instrukcji, a nawet jego napięcia pracy.

Tym razem jednak wykorzystamy go do ustawienia bitu `longmode` który pozwoli nam włączenie trybu 64 bitowego (ważne żeby przewracać ten bit po ustawieniu pagingu i PAE). 

Zapis tych rejestrów jest jeszcze bardziej skomplikowany bo z ich użyciem możemy zrobić o wiele więcej szkód. Aby odczytać rejestr MSR musimy znać jego adres i wprowadzić go do rejestru `ecx`, `mov ecx, 0xC0000080`. Pod adresem `0xC0000080` znajdziemy rejestr `MSR - EFER`, rozwinięcie skrutu to `model specific register - extended feature enable register`, trochę długa nazwa. Tłumacząc na polski, jest to rejestr którego używa się do załączenia rozszerzonych funkcjonalności procesora, właśnie takich jak tryb `longmode`.

Jak już posiadamy odpowiedni adres w rejestrze `ecx` możemy przejść do odczytu rejestru. Używamy do tego intrukcji `rdmsr`, instrukcja ta zwraca dane na dwóch rejestrach: `eax` oraz `edx`. Teraz trzeba ustawić odpowiedni bit, w tym przypadku bit 8, ustawiamy go przesunięciem bitowym a następnie zapisujemy rejestr instrukcją `wrmsr` (write msr). 

Teraz wystarczy załączyć paging w rejestrze CR0.

```x86asm
    ; włączenie pagingu w CR0
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax
```

Ostatecznie wystarczy wywołać dwie metody odpowiedzialne za paging i będzie on gotowy do użycia.
> plik: src/boot.asm, sekcja start po testach
```
<testy>
...
    call set_up_pageing
    call enable_paging    
...
```


I teraz możemy przejśść do trybu 64 bitowego... prawda?

Nie do końca... jest przed nami jeszcze dużo pracy, trzeba zdefiniować gdzie znajduje się kod.
Pomimo tego że w folderze z kodem tej części znajduje się już kod odpowiedzialny za implementację GTD, pozwolę sobie wytłumaczyć je w części z użyciem C.

