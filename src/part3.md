# Paging - Part 3

Paging, jest to metoda zarządzania pamięcią która oddziela pamięć wirtualną od pamięci fizycznej. Przestrzeń adresowa jest podzielona na równego rozmiaru "strony" i `page table` definiuje który adres wirtualny którego któremu  adresowi fizycznemu.

W `x86_64` rozmair strony to 4096 bajtów i 4 poziomiwe "strony", każda ze "stron" posiada 512, 8 bajtowych sektorów. 

![image](./photos/paging.png)

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