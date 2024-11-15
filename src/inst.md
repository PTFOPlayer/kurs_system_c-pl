# Używane instrukcje ASM

## Używany ASM
X86 jest dziwną architekturą... z jednej strony wszystko jest standaryzowane, a z drugiej mamy parę rodzajów assemblera. My będziemy krzystać z assemblera `NASM` z użyciem składni Intela.

## Dobre materiały do nauki NASM
- podstawowe informacje o nasm - https://wiki.osdev.org/NASM
- podstawowe informacje o assembly - https://wiki.osdev.org/Assembly
- dokumentacja `NASM` - https://www.nasm.us/docs.php

## Instrukcje
### mov
wykonuje kopię płytką (kopiuje dane bezpośrednie):
- rejestr - rejestr
- rejestr - pamięć (i vice versa)
- stała do pamięci/rejestru
```x86asm
mov dword[0xb8000], 0x2f4b2f4f
```
wstawia wartość `0x2f4b2f4f` o rozmiarze 32 bitów (`double word`) na andres pamięci `0xb8000`

```x86asm
mov rax, rbx
```
kopiuje `rbx` to `rax`
 
### cmp

dokonuje porównania na:
- rejestr - stała
- rejestr - rejestr
- rejestr - pamięć

Porównanie dokonywanie jest przy użyciu flag rejestru `flags`, `CF`, `ZF`, `PF`, uzyskując następujące możliwe typy porównań:
- `=`
- `<`
- `>`
- `<=`
- `>=`
- `!=`

```x86asm
    cmp eax, 0x36d76289
```

dokona porównania rejestru `eax` do wartości `0x36d76289`

### je, jne, jz, jnz

> wszystkie skoki warunkowe są wykonywane po wcześniejszym porównaniu

- `je` - skacze jeśli operandy są równe
- `jne` - skacze jeśli operandy są nierówne 
- `jz` - skacze jeśli jest ustawiona flaga `ZF` (zero)
- `jnz` - skacze jeśli nie jest ustawiona flaga `ZF` (zero)
  
```x86asm
    mov rcx, 0
loop:
    ...             ; zrób coś w pętli
    inc rcx         ; inkrementacja rejestru rcx
    cmp rcx, 10     ; porównanie
    je loop         ; skok warunkowy

    ...             ; kod po zakończeniu pętli
```
w powyższy sposób możemy zrealizować pętle która w C wyglądała by tak:
```C
for(int i = 0; i != 10; i++) {
    ...
}
```

### jl, jb, jle, jbe

> wszystkie skoki warunkowe są wykonywane po wcześniejszym porównaniu

- `jl` - skacze jeśli operand pierwszy jest mniejszy od drugiego, biorąc pod uwagę znak
- `jb` - skacze jeśli operand pierwszy jest mniejszy od drugiego, nie biorąc pod uwagę znaku
- `jle` - skacze jeśli operand pierwszy jest mniejszy bądź równy drugiemu, ze znakiem
- `jbe` - skacze jeśli operand pierwszy jest mniejszy bądź równy drugiemu, bez znaku

```x86asm
    mov rcx, 0
loop:
    ...             ; zrób coś w pętli
    inc rcx         ; inkrementacja rejestru rcx
    cmp rcx, 10
    jl loop

    ...             ; kod po zakończeniu pętli
```
w powyższy sposób możemy zrealizować pętle która w C wyglądała by tak:
```C
for(int i = 0; i < 10; i++) {
    ...
}
```

### jg, ja, jge, jae 

> wszystkie skoki warunkowe są wykonywane po wcześniejszym porównaniu

- `jg` - skacze jeśli operand pierwszy jest większy od drugiego, biorąc pod uwagę znak
- `ja` - skacze jeśli operand pierwszy jest większy od drugiego, nie biorąc pod uwagę znaku
- `jge` - skacze jeśli operand pierwszy jest większy bądź równy drugiemu, ze znakiem
- `jae` - skacze jeśli operand pierwszy jest większy bądź równy drugiemu, bez znaku

```x86asm
    mov rcx, 10
loop:
    ...             ; zrób coś w pętli
    dec rcx         ; dekrementacja rejestru rcx
    cmp rcx, 0
    jg loop

    ...             ; kod po zakończeniu pętli
```
w powyższy sposób możemy zrealizować pętle która w C wyglądała by tak:
```C
for(int i = 10; i > 0; i--) {
    ...
}
```

### call `<metoda>`

Wykonuje skok bezwarunkowy jednocześnie wypychając aktualny adres programu na stack w celu późniejszego powrotu. 

```x86asm
mov rax, 10
mov rbx, 20
call add
...             ; tutaj rax ma wartość 30
hlt             ; zatrzymanie programu aby uniknąć nieporządanego przejścia do `add`

add:
    add rax, rbx
    ret         ; instrukcja powrotu z metody
```

### and, or, xor, ...

Instrukcje logiczne, trzeba pamiętać że wykonują one operacje na każdym bicie wartości z osobna i nie zwracają wartości bool. Można je wykorzystać jako forma porównania.

```x86asm
mov al, 0b11110000
mov bl, 0b00111100

and al, bl
...         ; tutaj al ma wartość 0b00110000
```

### add, sub, mul, ...

Instrukcje arytmetyczne, w przypadku dzielenia trzeba pamiętać że dzielimy liczby całkowite i wynik skłąda się z wyniku i reszty, gdzie wynik jest wstawiany zawsze w rejestr `rax` a reszta `rdx` (lub mniejsze w przypadku używania mniejszych operacji)

```x86asm
mov rax, 10
mov rbx, 10

add rax, rbx 
...         ; rax ma wartość 20
```

```x86asm
xor rdx, rdx    ; czyszczenie rejestru rdx, xor jest wydajniejszy niż `mov rdx, 0`
mov rax, 81    
mov rbx, 10

div rbx         ; zawsze dzielimy rax przez jakąś wartość więc instrukcja przyjmuje jeden operand
...             ; rax ma teraz wartość 8 a rdx 1 
```

### inb, outb

instrukcje wejścia/wyjścia portów procesora (ex. 0x60 dla PS2)

```x86asm
outb 0x64, 0xff     ; wysłanie do portu 0x64 (ps2 control) wartości 0xff
```

## Pseudo instrukcje

- dq -> definiuje 64 bitową zmienną w miejscu wykonania pseudo instrukcji
- dd -> definiuje 32 bitową zmienną w miejscu wykonania pseudo instrukcji
- dw -> definiuje 16 bitową zmienną w miejscu wykonania pseudo instrukcji
- resb -> rezerwuje podaną ilość bajtów (ex: `resb 64`, rezerwuje 64 bajty)

## Rejestry

Hierarchia rejestrów jest dosyć prosta. W architekturze x86 szersze rejestry zawierają w sobie ich węższe odpowiedniki, na przykład `AX` (16 bit) składa się z `AL` i `AH` (8 bit) 

### Rejestry ogólne
| 64 bit | 32 bit | 16 bit | 8 bit - H   | 8 bit - L | Nazwa              |
| ------ | ------ | ------ | ----------- | --------- | ------------------ |
| RAX    | EAX    | AX     | AH          | AL        | Acumulator         |
| RBX    | EBX    | BX     | BH          | BL        | Base               |
| RCX    | ECX    | CX     | CH          | CL        | Counter            |
| RDX    | EDX    | DX     | DH          | DL        | Data               |
| RSI    | ESI    | SI     | nie dotyczy | SIL       | Source             |
| RDI    | EDI    | DI     | nie dotyczy | DIL       | Destination        |
| RSP    | ESP    | SP     | nie dotyczy | SPL       | Stack Pointer      |
| RBP    | EBP    | BP     | nie dotyczy | BPL       | Stack Base Pointer |

### Rejestr wskaźnika instrukcji
| 64-bit | 32-bit | 16-bit | Nazwa               |
| ------ | ------ | ------ | ------------------- |
| RIP    | EIP    | IP     | Instruction pointer |


### Rejestr EFLAGS (flagowy)
| Bit   | Label | Nazwa                                                 |
| ----- | ----- | ----------------------------------------------------- |
| 0     | CF    | Carry flag (przepełnienie)                            |
| 2     | PF    | Parity flag (parzystości)                             |
| 4     | AF    | Auxiliary flag                                        |
| 6     | ZF    | Zero flag (zero)                                      |
| 7     | SF    | Sign flag (znaku)                                     |
| 8     | TF    | Trap flag                                             |
| 9     | IF    | Interrupt enable flag (przerwań)                      |
| 10    | DF    | Direction flag                                        |
| 11    | OF    | Overflow flag (przepełnienia)                         |
| 12-13 | IOPL  | I/O privilege level                                   |
| 14    | NT    | Nested task flag                                      |
| 16    | RF    | Resume flag                                           |
| 17    | VM    | Virtual 8086 mode flag (tryb kompatybilności 8086)    |
| 18    | AC    | Alignment check                                       |
| 19    | VIF   | Virtual interrupt flag (wirtualne przerwanie)         |
| 20    | VIP   | Virtual interrupt pending (wirtualne przerwanie trwa) |
| 21    | ID    | Able to use CPUID instructio (CPUID istnieje)         |