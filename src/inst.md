# Używane instrukcje ASM

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
 

## Pseudo instrukcje

- dd -> wstawia w swoim miejscu w *binarce* 32 bity o podanej wartości
- dw -> wstawia w swoim miejscu w *binarce* 16 bitów o podanej wartości
