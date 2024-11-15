# Part 5 - minimalny driver vga

## Wstęp

VGA text mode, bo tym będziemy się zajmować, jest najbardziej podstawowym trybem wyświetlania jaki znajdziemy w komputerach x86. Mamy w nim mało imponujące 80 kolumn znaków i 25 wierszy.

## Mały stop na definicję int-ów

Osobiście nie lubię tego jak zdefiniowane są typy w C, rozmiary int-ów są trochę nie intuicyjne. Dlatego przygotowałem sobie prosty plik `int.h` który posiada definicje typów całkowitych, które w swojej nazwie posiadają informacje o ich rozmiarze.

> plik src/c/int.h
```c
#pragma once

typedef unsigned char u8;
typedef signed char i8;

typedef unsigned short u16;
typedef signed short i16;

typedef unsigned int u32;
typedef signed int i32;


typedef unsigned long int u64;
typedef signed long int i64;
```
## Zasada działania

VGA text mode posiada adres bazowy (jeden z adresów specjalnych) `0xb8000`, jest on tablicą danych mieszczącą 2000 wpisów każdy po 16 bitów. Pojedyńczy wpis składa się z dwóch bitów, pierwszy określa znak, drugi jego wygląd. 

Struktura wyglądu pojedyńczego znaku

| bity | funkcja     |
| ---- | ----------- |
| 0-3  | kolor znaku |
| 4-6  | kolor tła   |
| 7    | miganie     |

A więc mamy strukturę (w C):
```c
typedef struct VGAChar {
    char ascii_char;
    u8 color;
} VGAChar;
```



