# Rozdział 1

## Wprowadzenie

W momencie uruchomienia komputera BIOS wykonuje self test i rutyny inicjalizacyjne. Następnym krokiem jest przekazanie kontroli do bootloadera. Bootloader zajmuje się przejściem procesora w 32 bit protected mode (ponieważ procesor bootuje w 16 bit real mode) a następnie znajduje kernel na nośniku i ładuje go do pamięci.

My nie będziemy pisać bootloadera, jest to zbyt skomplikowane i czasochłonne, zamiast tego użyjemy GRUB2 (Multiboot2).

Kolejnymi krokami będą testy wykonywane przez kernel. My zdefiniujemy tylko 3 testy: Multiboot, CPUID oraz LongMode. Sprawdzimy w ten sposób czy nasz procesor posiada wszystkie potrzebne nam w przyszłości elementy i instrukcje. 

Do przejścia do trybu 64 bitowego potrzebujemy jeszcze schematu zarządzania pamięcią i adresację wirtualną. Zajmie się tym stronicowanie pamięci, pozwoli ono procesorowi na poprawne zarządzanie pamięcią oraz translację adresów wirtualnych do fizycznych.

Ostatnim etapem będzie wjeście w tryb 64 bitowy Long mode. Do tego potrzebujemy zdefinioawć tablicę deskryptorów (GDT) trybu 64 bitowego która będzie mówiła procesorowi gdzie znajdują się różne elementy naszego kernela oraz będzie posiadała segment stanu zadań (TSS) który pozwoli na przełączanie się między przerwaniami a głównym przebiegiem programu.

## Tabela pojęć

