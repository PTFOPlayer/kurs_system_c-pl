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

Teraz możemy taką strukturę wykorzystać do wpisania danych do naszego buforu:

```c

VGAChar *buffor = (VGAChar*)0xb8000;

VGAChar o = VGAChar {
    .ascii_char = 'o',
    .color = 0x0f,
};


VGAChar k = VGAChar {
    .ascii_char = 'k',
    .color = 0x0f,
};

buffor[0] = o;
buffor[1] = k;
```

W tym momencie możemy w dosyć wygodny sposób wypisywać znaki na ekran. Buffor traktujemy jako tablicę struktur `VGAChar`.

Niestety brakuje nam możliwości przechowania informacji o aktualnym położeniu na ekranie. Dlatego zdefiniujemy strukturę `Writer` w której będziemy przechowywali nasz bufor, razem z pozycjami, oraz domyślnym kolorem.

```c
typedef struct Writer {
    u8 position_width;
    u8 position_height;
    u8 default_color;
    VGAChar *buffer;
} Writer;
```

Pole `position_width` posiada informację o aktualnej kolumnie (jednej z 80), a `position_height` o wierszu (jeden z 25). W tym momencie możemy sobie definiować nasz `Writer` jako globalną zmienną statyczna. To pozwoli nam na napisanie prostego "wrappera" do wypisywania rzeczy na ekranie.

```c
static Writer writer = {
    .position_width = 0,
    .position_height = 0,
    .default_color = (Black << 4) | LightGray,
    .buffer = (VGAChar *)0xb8000,
};
```

## Implementacja podstawowych funkcji

A więc teraz jak już mamy globalną "instancję obiektu" która przechowuje informacje o tym co się znajduje na ekranie, możemy wokół niej pisać funkcje.

Będziemy implementować następujące funkcje, oraz zdefiniujemy sobie dwie stałe pomocnicze.
```c

const u8 BUFFER_HEIGHT = 25;
const u8 BUFFER_WIDTH = 80;

void kputnl(); // nowa linia
void kputs(char *); // wypisania string-a
void kputc(char); // wypisanie char-a
void kclear(); // czyszczenie ekranu
void kshift_buffer(); // przesunięcie buforu o wiersz w dół
```

---

Zaczniemy od funkcji czyszczącej, ponieważ będzie ona najprostrza w zrozumieniu.
```c
void kclear() {
    writer.position_width = 0;
    writer.position_height = 0;

    VGAChar empty = {.ascii_char = 0, .color = 0};

    for (u8 width = 0; width < BUFFER_WIDTH; width++)
        for (u8 height = 0; height < BUFFER_HEIGHT; height++)
            writer.buffer[height * BUFFER_WIDTH + width] = empty;
}
```
To co tu robimy to ustawienie aktualnej pozycji kursora na `(0,0)` a następnie zapisanie całego bufora pustymi znakami. Po wywołaniu tej funkcji jedyne co powinno być widać to czarny ekran i nic więcej.

---

Możemy już czyścić nasz ekran ale na nic to nam się nie przyda jeżeli nie będziemy mogli po wyczyszczeniu nic na nim wyświetlić. 

```c
void kputc(char ascii) {
    if (ascii == '\n') {
        kputnl();
        return;
    }

    u8 width = writer.position_width;
    u8 height = writer.position_height;

    VGAChar colored_char = {.ascii_char = ascii, .color = writer.default_color};

    writer.buffer[height * BUFFER_WIDTH + width] = colored_char;

    writer.position_width++;

    if (writer.position_width > BUFFER_WIDTH) kputnl(); 
    if (writer.position_height > BUFFER_HEIGHT) kshift_buffer();
}

void kputs(char *ascii) {
    while (*ascii != '\0') {
        kputc(*ascii);
        ascii++;
    }
}
```

Funckjsa `kputc` jest dosyć długa ale działa bardzo prosto. Pierwszą rzeczą którą musimy zrobić to sprawdzić czy nie mamy doczynienia z znakiem nowej linii, `'\n'`, niestety ale sprzęt nie realizuje nowej linii za nas i musimy sami ją zaimplementować co zrobimy później w funkcji `kputnl` która będzie wstawiała nową linię. 

Następnie konstruujemy z podanego znaku strukturę `VGAChar` którą wpisujemy na aktualną pozycję wpisywania.
```c
    VGAChar colored_char = {.ascii_char = ascii, .color = writer.default_color};

    writer.buffer[height * BUFFER_WIDTH + width] = colored_char;
```

Oczywiście musimy przenieść się pozycję dalej, więc musimy zinkrementować indeks kolumny w której się znajdujemy. Ale co nastąpi jak będziemy na kolumnie większej niż maksymalna? Musimy wziąć to pod uwagę, tak samo sytuacja w której będziemy na końcu ostatniego możliwego wiersza (to sprawdzimy w funkcji `kputnl`). Z tego powodu na koniec funkcji, po inkrementacji dodamy sprawdzenie czy przypadkiem nie zaczynamy przepełniać ilości wierszy.

```c
    writer.position_width++;    

    if (writer.position_width >= BUFFER_WIDTH) kputnl();
```

--- 