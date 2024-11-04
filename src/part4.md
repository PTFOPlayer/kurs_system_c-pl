# Part 4 - GDT, tryb 64 bit, wreszcie C

## Wprowadzenie
W tej cześci wreszcie wejdziemy w długo wyczekiwany tryb 64 bitowy. Wszystko jest już przygotowane oprócz jednej rzeczy... GDT.

## Czym jest GDT
GDT przechowuje informacje o konkretnych segmentach pamięci. Informacje te muszą być dostępne w każdym momencie dla procesor, ponieważ może on ich potrzebować przy np. wychodzeniu z rutyny przerwania. Grub ustawił już za nas gotowe GDT 32 bitowe którego moglibyśmy używać jeśli chcielibyśmy pozostać w trym trybie, ale żeby przejść do trybu 64 bitowego musimy zdefiniować 64 bitowe GDT.

## Wygląd GDT
GDT skłąda się z wielu wpisów, maksymalny rozmiar GDT to 65535 bajtów czyli 8192 wpisy (nitk w praktyce tylu nigdy nie używa), minimalna ilość to 2 wpisy: `NULL SEGMENT`, `CODE SEGMENT`, każdy wpis zajmuje 8 bajtów. GDT zawsze zaczyna się od `NULL SEGMENT`, jest to puste 8 bajtów które pozostają do dyspozycji dla procesora.

| Bity  | Nazwa      | Znaczenie                                                        |
| ----- | ---------- | ---------------------------------------------------------------- |
| 0-41  | -          | ignorowane                                                       |
| 42    | conforming | czy aktualny RING może być wyższy niż wpisany w sekcji privilege |
| 43    | executable | 1 dla segmentu kodu, 0 dla segmentu danych                       |
| 44    | descriptor | 1 dla segmentów kodu i danych, 0 dla innych                      |
| 45-46 | privilege  | 0 dla wpisu kernela, 3 dla wpisu użytkownika                     |
| 47    | present    | flaga obecności (zazwyczaj 1)                                    |
| 48-52 | ignored    | ignorowane                                                       |
| 53    | 64-bit     | w trybie 64 bit musi być 1                                       |
| 54    | 32-bit     | w trybie 64 bit musi być 0                                       |
| 55-63 | ignored    | ignorowane                                                       |

## Implementacja
Na początku trzeba zaimplementować `NULL SEGMENT`
> plik: ./src/boot.asm
```
...
section .rodata
gdt64:
    dq 0 
...
```
Bardzo proste prawda? Jest to po prostu zdefiniowanie zera w odpowiednim miejscu.

No to teraz `CODE SEGMENT`
```
...
section .rodata
gdt64:
    dq 0 
.code: equ $ - gdt64
    dq (1<<43) | (1<<44) | (1<<47) | (1<<53)
...
```


